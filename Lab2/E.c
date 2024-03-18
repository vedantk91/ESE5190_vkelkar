

#define F_CPU 16000000UL 
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

void initialize()
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_read()
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

int main(void)
{
	initialize();
	UART_init(BAUD_PRESCALER);
	char buffer[10];
	while (1)
	{
		uint16_t ADC = ADC_read();
		snprintf(buffer, sizeof(buffer), "%u\r\n", adc);
		UART_putstring(buffer);
		_delay_ms(100);  
	}
}