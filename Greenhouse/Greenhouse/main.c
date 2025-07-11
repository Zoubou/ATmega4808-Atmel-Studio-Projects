#include <avr/io.h>
#include <avr/interrupt.h>

#define THRESHOLD_LOW 64
#define THRESHOLD_HIGH 128

volatile uint16_t last_adc_value = 0;
int state = 0;

void ADC_init(void) {
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
	ADC0.CTRLA |= ADC_FREERUN_bm;
	ADC0.CTRLA |= ADC_ENABLE_bm;
	ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;
	ADC0.DBGCTRL |= ADC_DBGRUN_bm;
	ADC0.WINLT = THRESHOLD_LOW;  
	ADC0.WINHT = THRESHOLD_HIGH;  
	ADC0.INTCTRL |= ADC_WCMP_bm;
	ADC0.CTRLE = ADC_WINCM_OUTSIDE_gc;
	ADC0.COMMAND = ADC_STCONV_bm;  
}

void TCA0_init(uint16_t duration_ms) {
	TCA0.SINGLE.CTRLA = 0; 
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CMP0 = duration_ms;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | TCA_SINGLE_ENABLE_bm;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
	
}

int main(void) {
	PORTD.DIR |= PIN0_bm | PIN1_bm;      
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
	ADC_init();
	sei();

	while (1) {	
	}
}

ISR(ADC0_WCOMP_vect) {
	int intButtonflag = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intButtonflag;
	last_adc_value = ADC0.RES;
	ADC0.INTCTRL &= ~ADC_WCMP_bm;

	if (last_adc_value < THRESHOLD_LOW) {
		PORTD.OUTSET = PIN0_bm;
		state = 1;
	} else if (last_adc_value > THRESHOLD_HIGH) {
		PORTD.OUTSET = PIN1_bm;
		state = 2;
	} else {
		PORTD.OUTCLR = PIN0_bm | PIN1_bm;
		state = 0;
	}
}

ISR(TCA0_CMP0_vect) {
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm; 
	TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm; 
	PORTD.OUTCLR = PIN0_bm;  
	
	state = 0;
	ADC0.INTCTRL |= ADC_WCMP_bm;
}

ISR(PORTF_PORT_vect) {
	int int_flags = PORTF.INTFLAGS; // ???????? ??? INTFLAGS

	if (int_flags & PIN5_bm & state = 1) {
	    PORTF.INTFLAGS = PIN5_bm;
		uint16_t diff = THRESHOLD_LOW - last_adc_value;
		uint16_t time_ms = diff; 
		TCA0_init(time_ms);
	} else if (int_flags & PIN5_bm & state = 2) {
		
	} else {
		
	}
}