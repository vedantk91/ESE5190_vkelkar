#ifndef F_CPU
#define F_CPU 16000000
#endif //F_CPU

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)


static volatile uint32_t first_reading = 0;
static volatile uint32_t second_reading = 0;
static volatile uint32_t duty_cycle = 0;
char measure[20];

void initialize(){
	cli(); //clear prior interrupts
	
	DDRD |= (1<<PORTD6); //set PD6 as output
	DDRB |= (1<<PORTB3);
	TCCR0A = (1<<COM0A1)|(1<<COM0A0)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS01); //prescaler = 8 for timer 0
	OCR0A = 235; //10uS trigger pulse, 118uS off-time (128uS repetition rate) ON from 235 to 255. 16M/8/
	
	//Timer 1 running in normal mode
	DDRB &= ~(1<<PORTB0); //PB0 as input (ICP1)
	TCCR1B = (1<<ICNC1)|(1<<ICES1)|(1<<CS11); //timer 1 like C
	TCCR2A = (1<<COM2A0)|(1<<WGM20);  //timer 2 for buzzer, toggle when OCRA
	TCCR2B = (1<<CS22)|(1<<WGM22); //64 prescalar, phasecorrect for buzzer
	//OCR2A = 35;
	
	TIMSK1 |= (1<<ICIE1); //enable timer1 input capture interrupt
	sei();//enable global interrupts
}

uint32_t getDist(){
	static uint32_t echo_pulse_uS;
	static uint32_t distance_cm;
	//32768uS = 65536 clock ticks for Timer 1 with prescaler = 8
	echo_pulse_uS = (float)duty_cycle * 32768 / 65536;
	distance_cm = echo_pulse_uS * 0.034 / 2;
	
	return distance_cm;
}

ISR(TIMER1_CAPT_vect){
	if ((TCCR1B & (1<<ICES1)) == (1<<ICES1)){
		first_reading = ICR1;
	}
	else{
		second_reading = ICR1;
	}
	
	if (first_reading != 0 && second_reading != 0)
	{
		
		duty_cycle = second_reading - first_reading;
		first_reading = 0;
		second_reading = 0;
	}
	
	TCCR1B ^= (1<<ICES1); //toggle edge detection bit
	TIFR1 = (1<<ICF1);//clear Input Capture Flag
}
int main(){
	UART_init(BAUD_PRESCALER);
	initialize();
	while (1){
		
		uint32_t distance = getDist();
		OCR2A = 0.309*distance + 28.38;  //min value of OCRA is 29 and max is 59//min value measured is 2 max value measured is 99
		ultoa(distance,measure,10);
		strcat(measure,"\n");
		UART_putstring(measure);
		//_delay_ms(2000);
	}
	
}