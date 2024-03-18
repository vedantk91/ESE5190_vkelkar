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



void initialize(){
	UART_init(BAUD_PRESCALER);
	
	cli(); //clear prior interrupts
	DDRD |= (1<<DDD6); //set PD6 as output
	TCCR0A = (1<<COM0A1)|(1<<COM0A0)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS01); //prescaler = 8 for timer 0
	OCR0A = 235; //10uS trigger pulse
	//Timer 1 running in normal mode
	DDRB &= ~(1<<DDB0); //PB0 as input (ICP1)
	TCCR1B = (1<<ICNC1)|(1<<ICES1)|(1<<CS11); //positive edge detection for input capture and Prescaler = 8.
	sei();//enable global interrupts
	TIMSK1 |= (1<<ICIE1); //enable timer1 input capture interrupt
}

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
	TIFR1 = (1<<ICF1);//clear Input Capture Flag
}



int main(){
	UART_init(BAUD_PRESCALER);
	initialize();
	while (1){
		distance = getDist();
		sprintf(String,"%lu\n", distance);
		UART_putstring(String);
		_delay_ms(100);
	}
	
}