#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
//#include <util/delay.h>

void Initialize(){
	
	cli();

	TCCR1A &= ~(WGM10); //Timer 1 Normal Mode
	TCCR1A &= ~(WGM11);
	TCCR1B &= ~(WGM12);
	TCCR1B &= ~(WGM13);

	TCCR1B |= (1<<ICES1);  // Rising edge
	TCCR1B |= (1<<ICNC1);  // Noise Canceling
	
	TIMSK1 |= (1<<ICIE1); //enable input capture interrupt
	
	sei();
	
}

ISR(TIMER1_CAPT_vect)
{
	if(PINB & (1<<PINB0)){PORTB ^= (1<<PB5);}
	//else{PORTB &= ~(1<<PB5);}
	
	TIFR1 |= (1<<ICF1); //Clear Interrupt Flag
	
}

int main(void)
{
	
	DDRB |= (1 << DDB5);  // PB5 as output
	DDRB &= ~(1 << DDB0);  // PB0 as input
	PORTB |= (1<<PINB0); //internal pull up
	Initialize();
	while (1); // loop forever

}





