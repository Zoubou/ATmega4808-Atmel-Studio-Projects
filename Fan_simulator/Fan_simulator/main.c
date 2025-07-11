#include <avr/io.h>
#include <avr/interrupt.h>

// ??????? ????? ??? TCA0
#define TIMER_LPER_INITIAL  19    // (19+1)*1024/20e6 ? 1ms
#define BLADE_CMP_INITIAL  12    // 60% duty: (12/20 ? 60%) ??? Blade LED (PD0)

#define TIMER_HPER_INITIAL  38   // (38+1)*1024/20e6 ? 2ms
#define BASE_ON_TIME_INITIAL 16  // 40% duty ?? 2ms: (0.8ms / 51.2?s ? 16 ticks)

#define TIMER_LPER_SECOND   9     // (9+1)*1024/20e6 ? 0.5ms
#define BLADE_CMP_SECOND   5     // 50% duty: (5/10 ? 50%) ??? Blade LED (PD0)

volatile uint8_t button_state = 0;

void setupPWM(void) {
	
	PORTD.DIRSET = PIN0_bm | PIN1_bm;
	
	
	PORTF.DIRCLR = PIN6_bm;
	PORTF.PIN6CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
	
	TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV1024_gc;
	TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
	TCA0.SPLIT.HCNT = 2;

	TCA0.SPLIT.INTCTRL = TCA_SPLIT_HUNF_bm | TCA_SPLIT_LUNF_bm;
}

int main(void) {

	setupPWM();
	sei();
	
	while (1) {
		
	}
	
	return 0;
}

ISR(PORTF_PORT_vect) {
	PORTF.INTFLAGS = PIN6_bm;
	button_state++;
	
	if (button_state == 1) {
		TCA0.SPLIT.LPER = TIMER_LPER_INITIAL;
		TCA0.SPLIT.HPER = TIMER_HPER_INITIAL;
		TCA0.SPLIT.LCMP0 = BLADE_CMP_INITIAL;
		TCA0.SPLIT.HCMP0 = BASE_ON_TIME_INITIAL;
		TCA0.SPLIT.CTRLA |= TCA_SPLIT_ENABLE_bm;
		} else if (button_state == 2) {
		TCA0.SPLIT.LPER = TIMER_LPER_SECOND;
		TCA0.SPLIT.LCMP0 = BLADE_CMP_SECOND;
		} else if (button_state == 3) {
		TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
		PORTD.OUTCLR = PIN0_bm | PIN1_bm;
		button_state = 0;
	}
}

ISR(TCA0_LUNF_vect) {
	TCA0.SPLIT.INTFLAGS = TCA_SPLIT_LUNF_bm;
	PORTD.OUTTGL = PIN0_bm;
}

ISR(TCA0_HUNF_vect) {
	TCA0.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm;
	PORTD.OUTTGL = PIN1_bm;
}




