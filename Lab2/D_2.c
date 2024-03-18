#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
char String[25];

uint32_t distance;
volatile uint32_t first_pulse = 0;
volatile uint32_t second_pulse = 0;
volatile uint32_t duty_cycle = 0;
volatile int continuousMode = 0;

uint32_t getDist(){
	uint32_t echo_pulse_uS;
	uint32_t distance_cm;
	//32768uS = 65536 clock ticks for Timer 1 with prescaler = 8
	echo_pulse_uS = (float)duty_cycle * 32768 / 65536;
	distance_cm = echo_pulse_uS * 0.034 / 2;
	return distance_cm;
}

ISR(TIMER1_CAPT_vect){
	if ((TCCR1B & (1<<ICES1)) == (1<<ICES1)){
		first_pulse = ICR1;
	}
	else{
		second_pulse = ICR1;
	}
	if (first_pulse != 0 && second_pulse != 0){
		duty_cycle = second_pulse - first_pulse;
		first_pulse = 0;
		second_pulse = 0;
	}
	TCCR1B ^= (1<<ICES1); //toggle edge detection bit
	TIFR1 = (1<<ICF1);    //clear Input Capture Flag
}

void discrete()
{
	if(distance >= 2 && distance <= 9 )
	{
		OCR2A = 29;
	}
	if(distance >= 10 && distance <= 19 )
	{
		OCR2A = 30;
	}
	if(distance >= 20 && distance <= 29 )
	{
		OCR2A = 34;
	}
	if(distance >= 30 && distance <= 39 )
	{
		OCR2A = 38;
	}
	if(distance >= 40 && distance <= 49 )
	{
		OCR2A = 44;
	}
	if(distance >= 50 && distance <= 59 )
	{
		OCR2A = 46;
	}
	if(distance >= 60 && distance <= 69 )
	{
		OCR2A = 52;
	}
	if(distance >= 70 && distance <= 99 )
	{
		OCR2A = 59;
	}
}


void initialize(){
	UART_init(BAUD_PRESCALER);
		
	cli();

	//Setup as output pin
	DDRB |= (1 << DDB3);
	//Setup as input pin
	DDRD &= ~(1<<DDD7);
	PORTD |= (1<<PORTD7);
	
	//Timer 2
	TCCR2A = (1<<COM2A0)|(1<<WGM20);
	TCCR2B = (1<<CS22) | (1<<WGM22);
	
	//Ultrasonic/
	DDRD |= (1<<DDD6); //set PD6 as output
	TCCR0A = (1<<COM0A1)|(1<<COM0A0)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS01); //prescaler = 8 for timer 0
	OCR0A = 235; //10uS trigger pulse
	
	//Timer 1 running in normal mode
	DDRB &= ~(1<<DDB0); //PB0 as input (ICP1)
	TCCR1B = (1<<ICNC1)|(1<<ICES1)|(1<<CS11); //positive edge detection
	TIMSK1 |= (1<<ICIE1); //enable timer1 input capture interrupt
	
	
	sei(); //enable global interrupts
}

int main() {
	UART_init(BAUD_PRESCALER);
	initialize();
	while (1)
	{
		distance = getDist();
// 		sprintf(String,"The distance is %lu\n", distance);
// 		UART_putstring(String);
// 		sprintf(String,"The distance is %u\n", continuousMode);
// 		UART_putstring(String);
		
		if (continuousMode) {
			// Calculate the desired frequency based on the distance
	    	OCR2A = 0.309*distance + 28.38;  //min value of OCRA is 29 and max is 59//min value measured is 2 max value measured is 99	
// 			sprintf(String,"Continuous Mode");
// 			UART_putstring(String);
			} else {
			// Handle discrete mode logic here
			if (distance <= 120) {
				// Change the frequency index
				discrete();
// 				sprintf(String,"Discrete Mode");
// 				UART_putstring(String);
			}
		}

		_delay_ms(100);  // Delay between distance measurements and frequency changes

		// Check the button state and toggle mode if it's pressed
		if (!(PIND & (1<<PIND7))) {	
				continuousMode = !continuousMode;
		}
	}

	return 0;
	
}