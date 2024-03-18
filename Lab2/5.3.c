//433Hz

#define F_CPU 16000000UL
#include <avr/io.h>

void initialize() {
	// Set PD6 as output
	DDRD |= (1 << PD6); // this is OC0A internally linked to OCRA

	// Set Timer0 in CTC mode
	TCCR0A = (1 << WGM01);

	// Set prescaler to 256
	TCCR0B = (1 << CS02);

	// Set compare value for 440Hz
	OCR0A=70;
	
	TCCR0A |= (1 << COM0A0);  	// Toggle OC0A (PD6) on compare match

}

int main() {
	initialize();

	while(1) {
		
	}

	return 0;
}