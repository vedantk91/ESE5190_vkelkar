#include <avr/io.h>
#include<util/delay.h>
void setup()
{
	DDRB |= (1<<PORTB5);
	DDRB &= ~(1<<PORTB0);
}

int main(void)
{
	setup();
	PORTB |= (1<<PORTB0);
	while (1)
	{
		while(!(PINB &(1<<PINB0)))
		{
			PORTB |= (1<<PORTB5);
			_delay_ms(1000);
			PORTB &= ~(1<<PORTB5);	
		}	
	}
}