#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include <util/delay.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

unsigned int button_press_start = 0;
unsigned int button_press_end = 0;

char String[25];

char buffer[4]; // Morse code buffer
const char morseCodeTable[36][6] = {
	".-",    // A
	"-...",  // B
	"-.-.",  // C
	"-..",   // D
	".",     // E
	"..-.",  // F
	"--.",   // G
	"....",  // H
	"..",    // I
	".---",  // J
	"-.-",   // K
	".-..",  // L
	"--",    // M
	"-.",    // N
	"---",   // O
	".--.",  // P
	"--.-",  // Q
	".-.",   // R
	"...",   // S
	"-",     // T
	"..-",   // U
	"...-",  // V
	".--",   // W
	"-..-",  // X
	"-.--",  // Y
	"--..",  // Z
	"-----", // 0
	".----", // 1
	"..---", // 2
	"...--", // 3
	"....-", // 4
	".....", // 5
	"-....", // 6
	"--...", // 7
	"---..", // 8
	"----."  // 9
};

char buffer[4]; // Morse code buffer
volatile int bufferIndex = 0;
volatile char currentCharacter = ' ';

int main(void)
{
	cli();
	UART_init(BAUD_PRESCALER);
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB2);
	DDRB &= ~(1 << PB0);
	CLKPR |= (1 << CLKPCE);
	TCCR1B |= (1 << CS12)|(1<<CS10)|(1 << ICNC1)|(1 << ICES1);
	TIMSK1 |= (1<< ICIE1);
	sei();
	while (1) // loop forever
	{
		
		if((TCNT1 - button_press_end) > 45000)
		{
			TCNT1 = button_press_end;
			MorseCodeDecoder();
		}
	}
}

ISR(TIMER1_CAPT_vect)
{
	
	if(TCCR1B & (1<<ICES1))
	{
		button_press_start = ICR1;
	}
	else
	{
		button_press_end = ICR1;
		if((button_press_end- button_press_start) >= 468 && (button_press_end- button_press_start) <= 3000)
		{
		
			sprintf(String,". ");
			UART_putstring(String);
			buffer[bufferIndex++] = '.';
			PORTB |= (1 << PB1);
			_delay_ms(50);
		    PORTB &= ~(1 << PB1);

			
		}
		else if((button_press_end -button_press_start) >= 3000 && (button_press_end -button_press_start) <= 6250)
		{
			
			sprintf(String,"- ");
			UART_putstring(String);
			buffer[bufferIndex++] = '-';
			PORTB |= (1 << PB2);
		    _delay_ms(50);
			PORTB &= ~(1 << PB2);
		}
		
	}
	
	TCCR1B ^= (1<<ICES1);
// 	PORTB &= ~(1 << PB2);
// 	PORTB &= ~(1 << PB1);
	
}

void MorseCodeDecoder()
{
						
	int i;
	for (i = 0; i < 36; i++) {
	if (strcmp(buffer, morseCodeTable[i]) == 0) {
		currentCharacter = (i < 26) ? ('A' + i) : ('0' + (i - 26));
		UART_send(currentCharacter);
		break;
		i=0;
		button_press_end = 0;
		button_press_start = 0;
		}
	}
	
		bufferIndex = 0;
		buffer[3]=0;
		buffer[2]=0;
		buffer[1]=0;
		buffer[0]=0; //clearing buffer			
}