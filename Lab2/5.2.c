//437Hz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include <util/delay.h>

#define F_CPU 16000000UL
#include <util/delay.h>

void initialize()
{   cli();
	TCCR0A &= ~(WGM00);  //Timer 0 Normal Mode
	TCCR0A &= ~(WGM01);
	TCCR0B &= ~(WGM02);
	DDRD |= (1<<PORTD6);  //Output PD6
	TCCR0B |= (1<<CS02);  //prescalar 256 for timer0
	TCCR0B &= ~(1<<CS01);
	TCCR0B &= ~(1<<CS00);
	OCR0A = 70;           //initial value of OCR0A
	TIMSK0 = 1<< OCIE0A;  //O/p compare interrupt enable
	sei();
}
ISR(TIMER0_COMPA_vect)
{
	PORTD ^= (1<<PORTD6);             //Toggle D6 on output compare
	uint32_t OCR0A_updated = OCR0A;   //temporary variable for updating OCR0A
	OCR0A_updated+=70;                //increment OCR0A
	if(OCR0A_updated>255)
	OCR0A_updated = OCR0A_updated-255;   //handling the overflow condition
	OCR0A=OCR0A_updated;                 //storing in OCR0A
}
int main()
{
	initialize();
	
	while(1)
	{
		
	}
}