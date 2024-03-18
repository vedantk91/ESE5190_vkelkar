#include <avr/io.h>
// Define the CPU frequency (16 MHz for the Arduino).
// This is necessary for _delay_ms() to work properly.
#define F_CPU 16000000UL
#include <util/delay.h>
//uint8_t led_flag = 0;
int main(void)
{
	DDRB |= (1 << DDB1);  //B1 as output
	DDRD &= ~(1 << DDD7);  //D7 as input
	
	while (1) // loop forever
	{
		
     if(PIND & (1<<PIND7))
     {		
		PORTB |= (1 << PB1);  
     } 
	 
	 else {
		 PORTB &= ~(1 << PB1);;
	 }
		_delay_ms(200);
	}
}
