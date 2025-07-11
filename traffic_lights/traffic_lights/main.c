#include <avr/io.h>
#include <avr/interrupt.h>

#define T1 100  // Χρόνος διέλευσης τραμ
#define T2 25  // Χρόνος διάβασης πεζών
#define T3 50  // Χρόνος αδράνειας κουμπιού

volatile int flag_pedestrian = 0;

ISR(PORTF_PORT_vect) {
	if (flag_pedestrian == 0) {
		int flags = PORTF.INTFLAGS;
		PORTF.INTFLAGS = flags;
		flag_pedestrian = 1;
		TCA0.SPLIT.INTFLAGS = TCA_SPLIT_LUNF_bm;
		TCA0.SPLIT.LCNT = T2;
		TCA0.SPLIT.INTCTRL |= TCA_SPLIT_LUNF_bm;
	}
}

// ISR για T1 (χρονιστής τραμ)
ISR(TCA0_HUNF_vect) {
	flag_pedestrian = 1;
	TCA0.SPLIT.INTFLAGS = TCA_SPLIT_LUNF_bm;
	TCA0.SPLIT.LCNT = T2;
	TCA0.SPLIT.INTCTRL |= TCA_SPLIT_LUNF_bm;
	TCA0.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm;
	TCA0.SPLIT.HCNT = T1; 
}

// ISR για T2 (διάβαση πεζών)
ISR(TCA0_LUNF_vect) {
	flag_pedestrian = 2; 
	TCA0.SPLIT.INTCTRL &= ~TCA_SPLIT_LUNF_bm;
	TCA0.SPLIT.INTFLAGS = TCA_SPLIT_LCMP0_bm;
	TCA0.SPLIT.LCNT = T3; 
	TCA0.SPLIT.INTCTRL |= TCA_SPLIT_LCMP0_bm;
}

// ISR για T3 (αναμονή κουμπιού)
ISR(TCA0_LCMP0_vect) {
	flag_pedestrian = 0;
	TCA0.SPLIT.INTCTRL &= ~TCA_SPLIT_LCMP0_bm;  
}

int main(void) {
	PORTD.DIR |= 0b00000111; // PD0, PD1, PD2 ως έξοδοι για τα LED
	PORTD.OUT = 0b00000100; // LED πεζών κόκκινο αρχικά (PD0)
	
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc; 
	
	
	TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV1024_gc | TCA_SPLIT_ENABLE_bm;
	TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
	
	TCA0.SPLIT.HCNT = T1;
	TCA0.SPLIT.LCMP0 = 0;

	TCA0.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm ;

	sei(); 

	while (1) {
		if (flag_pedestrian == 1) { 
			PORTD.OUT = 0b00000001; // LED μεγάλου δρόμου κόκκινο 
			
		}
		else if (flag_pedestrian == 2) {
			PORTD.OUT = 0b00000100; // LED πεζών κόκκινο 
			
		}
	}
}
