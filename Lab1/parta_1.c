#include <avr/io.h>
// Define the CPU frequency (16 MHz for the Arduino).
// This is necessary for _delay_ms() to work properly.
#define F_CPU 16000000UL
#include <util/delay.h>
int main(void)
{
	// Set PB5 (pin 13) as OUTPUT.
	// This is equivalent to pinMode(13, OUTPUT) on the Arduino.
	DDRB |= (1 << PB3);
	DDRB |= (1 << PB2);
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB4);
	
	while (1) // loop forever
	{
		// Set pin 13 to HIGH. This is equivalent to digitalWrite(13, HIGH).
		PORTB |= (1 << PB3);
		PORTB |= (1 << PB2);
		PORTB |= (1 << PB1);
		PORTB |= (1 << PB4);

		_delay_ms(200);
		// Set pin 13 to LOW. This is equivalent to digitalWrite(13, LOW).
		//PORTB &= ~(1 << PB3);
		//_delay_ms(200);
	}
}
