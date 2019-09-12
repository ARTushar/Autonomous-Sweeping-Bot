#ifndef F_CPU
#define F_CPU 1000000UL // 1MHz clock speed
#endif
#define get_bit(reg,bitnum) ((reg & (1<<bitnum))>>bitnum)

#include <avr/io.h>
#include <util/delay.h>
#include "MotorDrive.h"
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>
#include "uart_transmission.h"

#define USART_BAUDRATE 9600

static volatile int pulse = 0;
static volatile int i = 0;
unsigned char previous_reading = 0;

int main(void)
{
	unsigned char count_l = 0;
	unsigned char count_r = 0;
	int16_t count_a = 0;
	DDRD |= (1 << DD3);
	DDRD &= ~(1 << DD2);

	GICR |= 1 << INT0;
	MCUCR |= 1 << ISC00;

	sei();
	init_motor_pins();
	UART_init(USART_BAUDRATE);
  // for reverse direction give negative value [-100, 100]
	set_speed(100, 100);
	while(1)
	{
		 PORTD |= 1<<PIND3;
		_delay_us(15);
		PORTD &= ~(1<<PIND3);
		 _delay_ms(50);
		count_a = pulse/58;
	
		if(count_a < 20){
			// rotate_right_90();
			// rotate_right_90();
			stop_left_motor();
			stop_right_motor();
		}
		else if(count_a >= 20){
			set_speed(100,100);
		}
		UART_transmit_char(1);
		count_r = UART_receive_char();
		UART_transmit_char(2);
		count_l = UART_receive_char();
		if(count_l < 20){
			stop_left_motor();
			// stop_right_motor();
		}
		else if(count_l >= 20){
			set_speed(100,get_right_speed());
		}
		if(count_r < 20){
			// stop_left_motor();
			stop_right_motor();
		}
		else if(count_r >= 20){
			set_speed(get_left_speed(),100);
		} 
	}
}

ISR(INT0_vect)
{
	if(i == 1)
	{
		TCCR1B = 0;
		pulse = TCNT1;
		TCNT1 = 0;
		i = 0;
	}

	if(i==0)
	{
		TCCR1B |= 1<<CS10;
		i = 1;
	}
}