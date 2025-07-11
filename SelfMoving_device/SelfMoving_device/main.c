#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define T1 40
#define T2 20  

int move = 0;
int degree_count = 360;   
int running = 1;
int direction = 0; 
int turn_count = 0;     

void Init_adc() {
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
	ADC0.CTRLA |= ADC_ENABLE_bm;
	ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc;
	ADC0.DBGCTRL |= ADC_DBGRUN_bm;
	ADC0.WINHT = 20;
	ADC0.WINLT = 10;
	ADC0.INTCTRL |= ADC_WCMP_bm;
	ADC0.CTRLE |= ADC_WINCM1_bm;
}

int main() {
	PORTD.DIR |= PIN1_bm;  // Forward
	PORTD.OUTCLR = PIN1_bm;
	
	PORTD.DIR |= PIN0_bm;  // Right
	PORTD.OUT |= PIN0_bm;
	
	PORTD.DIR |= PIN2_bm;  // Left
	PORTD.OUT |= PIN2_bm;
	
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	
	Init_adc();
	
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CTRLB = 0;
	TCA0.SINGLE.CMP0 = T1;
	TCA0.SINGLE.CMP1 = T2;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc;
	TCA0.SINGLE.CTRLA |= 1;
	
	ADC0.COMMAND |= ADC_STCONV_bm;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm;
	
	sei();
	
	while (running) {
	}
	cli();
}

ISR(ADC0_WCOMP_vect) {
	int intflags = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intflags;
	if(direction == 0){
		if (move == 0) { // Right turn
			PORTD.OUT = PIN0_bm;
			_delay_ms(5);
			PORTD.OUT = PIN1_bm;
			degree_count += 90; 
		}
		else if (move == 1) { // Left turn
			PORTD.OUT = PIN2_bm;
			_delay_ms(5);
			PORTD.OUT = PIN1_bm;
			degree_count -= 90; 
		}
	
		turn_count++;
	
		if (degree_count <= 0) {
			running = 0;
			PORTD.OUTCLR = PIN0_bm | PIN1_bm | PIN2_bm;
			TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_CMP1_bm;
			TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_CMP0_bm;
			ADC0.INTCTRL &= ~ADC_WCMP_bm; 
		
		}
	} else if(direction == 1){
		if (move == 0) { // left turn
			PORTD.OUT = PIN2_bm;
			_delay_ms(5);
			PORTD.OUT = PIN1_bm;
			degree_count -= 90;
		}
		else if (move == 1) { // right turn
			PORTD.OUT = PIN0_bm;
			_delay_ms(5);
			PORTD.OUT = PIN1_bm;
			degree_count += 90;
		}
		
		turn_count--;
		
		if (degree_count >= 360) {
			running = 0;
			PORTD.OUTCLR = PIN0_bm | PIN1_bm | PIN2_bm;
			TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_CMP1_bm;
			TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_CMP0_bm;
			ADC0.INTCTRL &= ~ADC_WCMP_bm;
			
		}
	}
}


ISR(TCA0_CMP1_vect){
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;
	TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_CMP1_bm;
	
	ADC0.CTRLE = ADC_WINCM0_bm; 
	ADC0.CTRLA |= ADC_FREERUN_bm;
	move = 1;
	
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_CMP0_bm;
	ADC0.COMMAND = ADC_STCONV_bm;
}

ISR(TCA0_CMP0_vect){
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;
	TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_CMP0_bm;
	
	ADC0.CTRLA &= ~ADC_FREERUN_bm;
	ADC0.CTRLE = ADC_WINCM1_bm; 
	move = 0;
	
	int intflag = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intflag;
	
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP1_bm;
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_CMP1_bm;
	ADC0.COMMAND = ADC_STCONV_bm;
}

ISR(PORTF_PORT_vect){
	//clear the interrupt flag
	int intButtonflag = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intButtonflag;
	
	if(direction == 0){
		direction = 1;
	} else if(direction ==1){
		direction = 0;
	}
}
