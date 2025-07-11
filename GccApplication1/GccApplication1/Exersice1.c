#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define del 10
 
int level = 0;
int error = 0;
 
int main() {
	PORTD.DIR |= 0b00000111; 
	PORTD.OUT &= ~0b00000111; 
	
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	PORTF.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	
	sei();
	 
	while (1){
		if (level == 0 && error == 0){
			PORTD.OUT = 0b00000000;
		}
		else if (level == 1 && error == 0){
			PORTD.OUT = 0b00000010;
		}
		else if (level == 2 && error == 0){
			PORTD.OUT = 0b00000100;
		}
		else if (error == 1){
			PORTD.OUT = 0b00000001;
			error = 0;
		}
	}	 
}

ISR(PORTF_PORT_vect){
	if ((PORTF.INTFLAGS & PIN5_bm) && !(PORTF.INTFLAGS & PIN6_bm)){
		if (level ==0 || level == 1){
			level += 1;
		}
	    PORTF.INTFLAGS = PIN5_bm;
	}
	else if ((PORTF.INTFLAGS & PIN6_bm) && !(PORTF.INTFLAGS & PIN5_bm)){
		if (level ==1 || level == 2){
			level -= 1;
		}
		PORTF.INTFLAGS = PIN6_bm;
	}
	else if ((PORTF.INTFLAGS & PIN5_bm) && (PORTF.INTFLAGS & PIN6_bm)){
		error = 1;
		PORTF.INTFLAGS = PIN5_bm;
		PORTF.INTFLAGS = PIN6_bm;
	}
}