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
#define RIGHT_ANGLE_ROTATION_DELAY 3450
#define SPEED_CM_PER_SEC 27.14

static volatile int pulse = 0;
static volatile int i = 0;
unsigned char previous_reading = 0;
unsigned char map[10][10];
int room_max_length=0;
int room_max_width=0;
int current_distance_to_cover = 0;
int current_distance_covered = 0;

int main(void)
{
	int16_t rotation_flag = 0;

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

	PORTD |= 1<<PIND3;
	_delay_us(15);
	PORTD &= ~(1<<PIND3);
	_delay_ms(50);
	count_a = pulse/58;
	room_max_length = count_a;
	current_distance_to_cover = count_a;

	UART_transmit_char(1);
	count_r = UART_receive_char();
	UART_transmit_char(2);
	count_l = UART_receive_char();
	room_max_width = ((count_r>count_l) ? count_r : count_l);

	set_speed(70, 70);

	while(1)
	{
		PORTD |= 1<<PIND3;
		_delay_us(15);
		PORTD &= ~(1<<PIND3);
		 _delay_ms(50);
		count_a = pulse/58;
		
		if(count_a > room_max_length) room_max_length = count_a;
	
		if(count_a < 20){
			stop_left_motor();
			stop_right_motor();
			UART_transmit_char(1);
			count_r = UART_receive_char();
			UART_transmit_char(2);
			count_l = UART_receive_char();

			if(rotation_flag==0) {
				if(count_r>count_l) {
					room_max_width = count_r;
					set_speed(70,5);				// right turn
					_delay_ms(2*RIGHT_ANGLE_ROTATION_DELAY);
					rotation_flag = 1;
					set_speed(70,70);
				} else {
					room_max_width = count_l;
					set_speed(5,70);				// left turn
					_delay_ms(2*RIGHT_ANGLE_ROTATION_DELAY);
					rotation_flag = 2;
					set_speed(70,70);
				}
			}
			else if(rotation_flag==1) {
				if(count_l>20) {
					set_speed(5,70);			// left turn
					_delay_ms(RIGHT_ANGLE_ROTATION_DELAY);
					rotation_flag = 2;
				} else {
					stop_left_motor();
					stop_right_motor();
					rotation_flag = 4;
				}
			}
			else if(rotation_flag==2) {
				if(count_r>20) {
					set_speed(70,5);			// right turn
					_delay_ms(RIGHT_ANGLE_ROTATION_DELAY);
					rotation_flag = 1;
				} else {
					stop_left_motor();
					stop_right_motor();
					rotation_flag = 4;
				}
			}
		} 
		else if(count_a >= 20){
			set_speed(70,70);
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