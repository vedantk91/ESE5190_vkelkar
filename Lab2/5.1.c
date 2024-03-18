
#include <avr/io.h>
#include <avr/interrupt.h>

void setupTimer0() {
	
	cli();
	// Set PD6 (Arduino digital pin 6) as an output
	DDRD |= (1 << PD6);
    PORTD |= (1 << PD6);
	
	// Initialize Timer0 in normal mode
		TCCR0A &= ~(WGM00); //Timer 1 Normal Mode
		TCCR0A &= ~(WGM01);
		TCCR0B &= ~(WGM02);
	TCCR0B = (1 << CS02) ; // Prescaler 256
	TIMSK0 = (1 << TOIE0); // Enable Timer0 Overflow Interrupt
	
	
	sei();
}

int main(void)
{
	setupTimer0();
    /* Replace with your application code */
    while (1) 
    {
    }
}

ISR(TIMER0_OVF_vect) {
	// Toggle PD6 (digital pin 6)
	PORTD ^= (1 << PD6);
}