#include <avr/io.h>
// Define the CPU frequency (16 MHz for the Arduino).
// This is necessary for _delay_ms() to work properly.
#define F_CPU 16000000UL
#include <util/delay.h>
//uint8_t led_flag = 0;

int led_counter=0;
int main(void)
{
	DDRB |= (1 << DDB1);  //B1 as output
	DDRB |= (1 << DDB2);  //B2 as output
	DDRB |= (1 << DDB3);  //B3 as output
	DDRB |= (1 << DDB4);  //B4 as output
	DDRD &= ~(1 << DDD7);  //D7 as input
	
	PORTB |= (1 << PB1);
	
	while (1) // loop forever
	{	
     if(PIND & (1<<PIND7))
     {		
		led_counter++;
		if(led_counter>=4){led_counter=0;}
		_delay_ms(50);
     }  
      if(led_counter==0){
			 
	     PORTB |= (1 << PB1);
	     PORTB &= ~(1 << PB2);
		 PORTB &= ~(1 << PB3);
		 PORTB &= ~(1 << PB4);
		 } 
	 if(led_counter==1){
		 
		 PORTB |= (1 << PB2);
		 PORTB &= ~(1 << PB1);
	     } 	 
	 else if(led_counter==2){
		 	 
		 PORTB |= (1 << PB3);
		 PORTB &= ~(1 << PB2);
	 	 }	 
	else if(led_counter==3){
			  	 
		 PORTB |= (1 << PB4);
		 PORTB &= ~(1 << PB3);
		 }
		_delay_ms(50);
	}
}
