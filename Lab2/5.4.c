//442 16M/256/440/4

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

void initialize(){
	cli();

	//Setup as output pin
	DDRD |= (1<<PORTD6); //Output
	TCCR0B |= (1<<CS02); //prescalar 256
	TCCR0B &= ~(1<<CS01);
	TCCR0B &= ~(1<<CS00);
	//Timer 0 Phase correct, inverted PWM
	TCCR0A |= (1<<WGM00);
	TCCR0A &= ~(1<<WGM01);
	TCCR0B |= (1<<WGM02);
	OCR0A = 35;
	//Toggle OC0A on compare match.
	TCCR0A |= (1<<COM0A0);
	TCCR0A &= ~(1<<COM0A1);
	
	sei();
}

int main() {
	initialize();
	while (1)
	{
		
	}
	return 0;
}


