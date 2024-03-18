#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include <util/delay.h>
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#define BUTTON_PIN      PB0
#define DOT_MIN_DURATION_MS 1875
#define DASH_MIN_DURATION_MS 12500
#define DASH_MAX_DURATION_MS 25000
#define SPACE_MAX_DURATION_MS 25000

volatile uint16_t button_press_start = 0;
volatile uint16_t button_press_end = 0;
volatile char c; // Store up to 6 dots/dashes (max Morse code length)
uint8_t space_flag=0;
uint16_t overflow_count;

void initialize() {
	// Configure BUTTON_PIN as input with pull-up resistor
	DDRB &= ~(1 << BUTTON_PIN);
	PORTB |= (1 << BUTTON_PIN);

	// Configure Input Capture
	TCCR1B &= ~(1 << ICES1); // Set to capture falling edge
	TCCR1B |= (1 << CS12); // Set prescaler to 256
	TCCR1B &= ~(1 << CS11);
	TCCR1B &= ~(1 << CS10);
	TIMSK1 |= (1 << ICIE1); // Enable Input Capture Interrupt
	TIMSK1 |= (1<<TOIE1);
	sei(); // Enable global interrupts
}

int main() {
	initialize();
	UART_init(BAUD_PRESCALER);

	while (1) {

	}
	return 0;
}

ISR(TIMER1_CAPT_vect) {
	if (TCCR1B & (1<<ICES1))
	
	{
		
		button_press_start = ICR1; // Capture the end time
		//	c = 'i';
		//	UART_send(c);
		space_flag=1;
		
	}
	
	else {
		
		button_press_end = ICR1; // Capture the start time. The input capture is updated with the counter (TCNT1) value each time an event occurs on the ICP1 pin
		uint16_t duration = button_press_end - button_press_start;
		if(duration<0)
		{
			duration = duration+62500;
		}
		else if (duration >= DOT_MIN_DURATION_MS && duration < DASH_MIN_DURATION_MS) {
			c = '.';
			UART_send(c);
		}
		else if (duration >= DASH_MIN_DURATION_MS && duration <= DASH_MAX_DURATION_MS) {
			c = '-';
			UART_send(c);
		}
		
		space_flag=0;
		//	c = 'e';
		//	UART_send(c);
	}
	
	TCCR1B ^= (1<<ICES1); //flip rising to falling edge after one detection
}

ISR(TIMER1_OVF_vect)
{
	overflow_count++;
	uint32_t current_time = ((uint32_t)overflow_count << 16) + TCNT1;
	if(current_time - button_press_end >25000 && space_flag==0 ) {
		UART_putstring(" ");
		button_press_end = current_time;
	}
}