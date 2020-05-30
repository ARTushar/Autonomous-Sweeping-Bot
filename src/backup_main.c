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
#define RIGHT_RIGHT_ANGLE_ROTATION_DELAY 1850
#define LEFT_RIGHT_ANGLE_ROTATION_DELAY 1900
#define RIGHT_SPEED_OFFSET 70
#define LEFT_SPEED_OFFSET 66

static volatile int pulse = 0;
static volatile int i = 0;
unsigned char previous_reading = 0;

int main(void)
{
	int8_t rotation_flag = 0;
	int8_t rotation_flag2 = 0;

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
	set_speed(LEFT_SPEED_OFFSET, RIGHT_SPEED_OFFSET);
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
			UART_transmit_char(1);
			count_r = UART_receive_char();
			UART_transmit_char(2);
			count_l = UART_receive_char();

			if(rotation_flag==0) {
				if(count_r>count_l) {
					set_speed(LEFT_SPEED_OFFSET,5);
					_delay_ms(2*RIGHT_RIGHT_ANGLE_ROTATION_DELAY);
					rotation_flag = 1;
					set_speed(LEFT_SPEED_OFFSET,RIGHT_SPEED_OFFSET);
				} else {
					set_speed(5,RIGHT_SPEED_OFFSET);
					_delay_ms(2*LEFT_RIGHT_ANGLE_ROTATION_DELAY);
					rotation_flag = 2;
					set_speed(LEFT_SPEED_OFFSET,RIGHT_SPEED_OFFSET);
				}
			}
			else if(rotation_flag==1) {
				if(count_l>20) {
					set_speed(5,RIGHT_SPEED_OFFSET);
					_delay_ms(2*LEFT_RIGHT_ANGLE_ROTATION_DELAY);
					rotation_flag = 2;
					set_speed(LEFT_SPEED_OFFSET,RIGHT_SPEED_OFFSET);
				}
				else
				{
					stop_left_motor();
					stop_right_motor();
					rotation_flag = 4;
				}
				 
			}
			else if(rotation_flag==2) {
				if(count_r>20) {
					set_speed(LEFT_SPEED_OFFSET,5);
					_delay_ms(2*RIGHT_RIGHT_ANGLE_ROTATION_DELAY);
					rotation_flag = 1;
					set_speed(LEFT_SPEED_OFFSET,RIGHT_SPEED_OFFSET);
				}
				else
				{
					stop_left_motor();
					stop_right_motor();
					rotation_flag = 4;
				} 
			}
			else
			{
				stop_right_motor();
				stop_left_motor();
			}
		} 
		else if(count_a >= 20){
			//rotation_flag = rotation_flag2;
			set_speed(LEFT_SPEED_OFFSET,RIGHT_SPEED_OFFSET);
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