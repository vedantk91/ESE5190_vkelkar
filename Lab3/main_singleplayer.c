

#define F_CPU 16000000UL
#include <util/delay.h>
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"
#include "ST7735.h"
#include "LCD_GFX.h"
#include <avr/interrupt.h>

char buffer[25];
int16_t x_paddle=0;
uint16_t del_x_paddle=0;
int16_t user_paddle=0;
uint16_t del_user_paddle=0;
uint16_t speed=10;
int16_t y_circle=50;
int16_t x_circle=50;
int16_t circle_radius=7;
int16_t x_circle_speed=10;
int16_t y_circle_speed=10;
uint16_t CPU_score=0;
uint16_t user_score=0;

#define WIFI_CTRL


volatile uint16_t adcval=0xFFFF;

void start_timer() {
	TCCR1B &= ~(1 << CS10);
	TCCR1B |= (1 << CS11);
	TCCR1B &= ~(1 << CS12);
}

void stop_timer()
{
	TCCR1B &= (1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B &= ~(1 << CS12);
}

void enable_pwm()
{
	TCCR0A |= (1<<5); // Enable PWM
}

void disable_pwm()
{
	TCCR0A &= ~(1<<5); // Disable PWM
}

void Initialize()
{
	DDRD|=(0x1 << 4);
	DDRD |= (0x1 << 5);
	DDRD |= (1<<6);
	DDRD|=(0x1 << 7);
	
	DDRB &= ~(1 << PB3);
	DDRB &= ~(1 << PB4);
	PORTB |= (1 << PB3);
	PORTB |= (1 << PB4);
	
	TCCR0B = 0;
	TCCR0B |= 4; //Set prescaler to 256
	TIFR0 |= (1<<0); // clear interrupt flag
	TCNT0 = 0;
	
	//Set COM0A1:COM0A0 to 0 so that OC0A is not used
	TCCR0A &= ~(1<<7);
	TCCR0A &=~(1<<6);
	
	//Set COM0B1:COM0B0 to 0. TCCR0A |= (1<<5); will be called to activate the PWM.
	TCCR0A &= ~(1<<5);
	TCCR0A &= ~(1<<4);
	
	TCCR0A &= ~(1<<WGM00);   // Set WGM00
	TCCR0A &= ~(1<<WGM01);  // Clear WGM01
	TCCR0B &= ~(1<<WGM02);   // Set WGM02
	
	cli();
	
	OCR0A = 29;
	OCR0B = 10;
	
	TIMSK1 |= (1<<TOIE1);
	TIFR1 |= (1<<TOV1);
	stop_timer();
	
	sei();
	
	lcd_init();
}

void ACD_int()
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_read()
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	ADCSRA |= (1 << ADIF);
	return ADC;
}

void init_adc()
{
    PRR &= ~(1<<PRADC);

    ADMUX |= (1<<REFS0);
    ADMUX &= ~(1<<REFS1);
    
    ADCSRA |= (1<<ADPS0);
    ADCSRA |= (1<<ADPS1);
    ADCSRA |= (1<<ADPS2);

    ADMUX &= ~(1<<MUX0);
    ADMUX &= ~(1<<MUX1);
    ADMUX &= ~(1<<MUX2);
    ADMUX &= ~(1<<MUX3);

    ADCSRA |= (1<<ADATE);

    ADCSRB &= ~(1<<ADTS0);
    ADCSRB &= ~(1<<ADTS1);
    ADCSRB &= ~(1<<ADTS2);

    DIDR0 |= (1<<ADC0D);

    ADCSRA |= (1<<ADEN);

    ADCSRA |= (1<<ADIE);

    ADCSRA |= (1<<ADSC);

}

ISR(TIMER1_OVF_vect)
{
	disable_pwm();
	stop_timer();
}

ISR(ADC_vect)
{
    adcval = ADC;
}

int led_on_count=0;
char cpu[6]="CPU ";
char player[8]="Player ";
volatile int button1state;
volatile int button2state;

char debug_string[20];

int main(void)
{
	Initialize();
	LCD_setScreen(BLACK);
	init_adc();
	UART_init(BAUD_PRESCALER);
	LCD_drawCircle(x_circle,y_circle,circle_radius,YELLOW,BLACK);

    while (1) 
    {
		LCD_drawString(40,10,cpu,WHITE,BLACK);
		LCD_drawString(100,10,player,WHITE,BLACK);
		char score[5];
		sprintf(score, "%d", CPU_score);
		LCD_drawString(40,20,score,WHITE,BLACK);
		sprintf(score, "%d", user_score);
		LCD_drawString(100,20,score,WHITE,BLACK);

		LCD_drawBlock(x_circle-(circle_radius),y_circle-(circle_radius),x_circle+(circle_radius),y_circle+(circle_radius),BLACK);
		x_circle += x_circle_speed;
		y_circle += y_circle_speed;
		
		if(x_circle+(circle_radius+5)>=LCD_WIDTH || x_circle<=circle_radius+5)
		{
			x_circle_speed*=-1;
			start_timer();
			enable_pwm();
		}
			
		if(y_circle+(circle_radius+5)>=LCD_HEIGHT || y_circle<=circle_radius+5)
		{
			if(y_circle+(circle_radius+5)>=LCD_HEIGHT)
			{
				if(!(x_circle>=user_paddle && x_circle<=user_paddle+20))
				{
					CPU_score++;
					PORTD |= (1<<4);
					PORTD &= ~(1<<7);
				}
				start_timer();
				enable_pwm();
				y_circle=LCD_HEIGHT-(circle_radius+7);
			}
			else if(y_circle<=circle_radius+5)
			{
				if(!(x_circle>=x_paddle && x_circle<=x_paddle+20))
				{
					user_score++;
					PORTD &= ~(1<<4);
					PORTD |= (1<<7);
				}
				start_timer();
				enable_pwm();
				y_circle=circle_radius+7;
			}

			y_circle_speed*=-1;
			
		}
		
		LCD_drawCircle(x_circle,y_circle,circle_radius,YELLOW,BLACK);
		
		_delay_ms(100);
        x_paddle += del_x_paddle;

        if(x_paddle>=139) {
			x_paddle=139;
            del_x_paddle = -speed;
        }
        if(x_paddle<=0) {
			x_paddle=0;
            del_x_paddle = speed;
        }

        LCD_drawBlock(0,0,x_paddle,5,BLACK);
        LCD_drawBlock(x_paddle,0,x_paddle+20,5,CYAN);
        LCD_drawBlock(x_paddle+20,0,159,5,BLACK);

		button1state = (PINB & (1<<4));
		button2state = (PIND & (1<<3));

        if(adcval > 600 || button1state) {
            del_user_paddle = speed;
        }
        else if (adcval < 400 || button2state) {
            del_user_paddle = -speed;
        }
        else {
            del_user_paddle = 0;
        }
        user_paddle += del_user_paddle;
        if(user_paddle > 139)
            user_paddle = 139;
        if(user_paddle < 0)
            user_paddle = 0;
        
        LCD_drawBlock(0,122,user_paddle,127,BLACK);
        LCD_drawBlock(user_paddle,122,user_paddle+20,127,CYAN);
        LCD_drawBlock(user_paddle+20,122,159,127,BLACK);
		
        if(user_score == 6){
            char print_str[20] = "PLAYER WINS!";
            LCD_setScreen(BLACK);
            LCD_drawString(50,60,print_str,WHITE,BLACK);
            _delay_ms(3000);
            LCD_setScreen(BLACK);
            user_score=0;
            CPU_score=0;
        }
        else if(CPU_score == 6) {
            char print_str[20] = "CPU WINS!";
            LCD_setScreen(BLACK);
            LCD_drawString(50,60,print_str,WHITE,BLACK);
            _delay_ms(3000);
            LCD_setScreen(BLACK);
            user_score=0;
            CPU_score=0;
        }
    }
	
}
