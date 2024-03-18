/*
 * main.c
 *
 * Created: 19-10-2023 16:23:44
 * Author : Vedant
 */ 

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


ISR(TIMER2_COMPB_vect)
{
	if((PINB & (1 << PORTB3))== (1>>PORTB3))
	{
		PORTB ^= (1<<PORTB5);
	}
	else
	{
		PORTB &= ~(1 << PORTB5);
	}
}

ISR(TIMER1_CAPT_vect){
	
	if ((TCCR1B & (1<<ICES1)) == (1<<ICES1))
	{
		first_pulse = ICR1;
	}
	else
	{
		second_pulse = ICR1;
	}
	if (first_pulse != 0 && second_pulse != 0)
	{
		duty_cycle = second_pulse - first_pulse;
		first_pulse = 0;
		second_pulse = 0;
	}
	TCCR1B ^= (1<<ICES1);            //toggle edge detection bit
	TIFR1 = (1<<ICF1);               //clear Input Capture Flag
}

void duty_cycle_variation()         //thresholds for volume variation according to ADC values
{
	if(ADC <= 860 && ADC >= 775)
	{
		OCR2B = 0.5*OCR2A;
	}
	else if(ADC <= 774 && ADC >= 689)
	{
		OCR2B = 0.45*OCR2A;
	}
	else if(ADC <= 688 && ADC >= 603)
	{
		OCR2B = 0.4*OCR2A;
	}
	else if(ADC <= 602 && ADC >= 517)
	{
		OCR2B = 0.35*OCR2A;
	}
	else if(ADC <= 516 && ADC >= 431)
	{
		OCR2B = 0.3*OCR2A;
	}
	else if(ADC <= 430 && ADC >= 345)
	{
		OCR2B = 0.25*OCR2A;
	}
	else if(ADC <= 344 && ADC >= 259)
	{
		OCR2B = 0.20*OCR2A;
	}
	else if(ADC <= 258 && ADC >= 173)
	{
		OCR2B = 0.15*OCR2A;
	}
	else if(ADC <= 172 && ADC >= 86)
	{
		OCR2B = 0.10*OCR2A;
	}
	else if(ADC < 86)
	{
		OCR2B = 0.05*OCR2A;
	}
}

void discrete()
{
	if(distance >= 2 && distance <= 9 )
	{
		OCR2A = 29;
		duty_cycle_variation();
	}
	if(distance >= 10 && distance <= 19 )
	{
		OCR2A = 30;
		duty_cycle_variation();
	}
	if(distance >= 20 && distance <= 29 )
	{
		OCR2A = 34;
		duty_cycle_variation();
	}
	if(distance >= 30 && distance <= 39 )
	{
		OCR2A = 38;
		duty_cycle_variation();
	}
	if(distance >= 40 && distance <= 49 )
	{
		OCR2A = 44;
		duty_cycle_variation();
	}
	if(distance >= 50 && distance <= 59 )
	{
		OCR2A = 46;
		duty_cycle_variation();
	}
	if(distance >= 60 && distance <= 69 )
	{
		OCR2A = 52;
		duty_cycle_variation();
	}
	if(distance >= 70 && distance <= 99 )
	{
		OCR2A = 59;
		duty_cycle_variation();
	}
}


uint32_t getDist(){
	
	uint32_t echo_pulse_uS;
	uint32_t distance_cm;
	echo_pulse_uS = (float)duty_cycle * 32768 / 65536;   	//32768uS = 65536 clock ticks for Timer 1 with prescaler = 8
	distance_cm = echo_pulse_uS * 0.034 / 2;                //formula for calculating Distance in cm
	return distance_cm;                                     //distance in centimeters
}

void initialize(){
	
	UART_init(BAUD_PRESCALER);
		
	cli();

    // Set reference to AVCC and input channel to PA0 (ADC0)
   	ADMUX = (1 << REFS0);
    
   	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable the ADC and set prescaler to 128 (16MHz/128 = 125kHz)
  	DIDR0 |= (1<<ADC5D);
		
	//Setup as output pin
	DDRB |= (1 << DDB3);
	DDRB |= (1<<PORTB5);
	
	//Setup as input pin
	DDRD &= ~(1<<DDD7);
	PORTD |= (1<<PORTD7);
	
	//Timer 2
	TCCR2A = (1<<COM2A0)|(1<<WGM20);  //Phase correct PWM for Timer2
	TCCR2B = (1<<CS22) | (1<<WGM22);  //prescalar 64 for timer2
	
	//Ultrasonic/
	DDRD |= (1<<DDD6);                                        //set PD6 as output trigger
	TCCR0A = (1<<COM0A1)|(1<<COM0A0)|(1<<WGM01)|(1<<WGM00);   //Fast PWM for Timer0
	TCCR0B |= (1<<CS01);                                      //prescaler = 8 for timer 0
	OCR0A = 235;                                              //10uS trigger pulse
	
	TCCR1A &= ~(WGM10);                                       //Timer 1 running in normal mode 
	TCCR1A &= ~(WGM11);
	TCCR1B &= ~(WGM12);
	
	DDRB &= ~(1<<DDB0);                          //PB0 as input (ICP1)
	TCCR1B = (1<<ICNC1)|(1<<ICES1)|(1<<CS11);    //positive edge detection and prescalar 8 for timer 1
	TIMSK1 |= (1<<ICIE1);                        //enable timer1 input capture interrupt
	TIMSK2 |= (1<<OCIE2B);                       //Enable output compare interrupt for timer2

	sei(); //enable global interrupts
}

int main() {
	UART_init(BAUD_PRESCALER);
	initialize();
	while (1)
	{
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));          //helps fetch ADC values
		
		distance = getDist();                  //run the function to calculate distance
		
 		sprintf(String,"%u\n", continuousMode); //check if continuos mode or discrete for debugging
 		UART_putstring(String);
		
		if (continuousMode) 
		{
			// Calculate the desired frequency based on the distance
	    	OCR2A = 0.309*distance + 28.38;       //min value of OCRA is 29 and max is 59//min value measured is 2 max value measured is 99	
			duty_cycle_variation();               //duty cycle will vary depending on the light intensity

		} 
		else 
		{
			if (distance <= 99)
			 {
				discrete();            //run the discrete mode calculations
			 }
	    }

		_delay_ms(100);  
		
		if (!(PIND & (1<<PIND7)))        // Check the button state and toggle mode if it's pressed
		{	
				continuousMode = !continuousMode;     //Toggle the mode
		}
	}
	return 0;
}