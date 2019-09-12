#include <avr/io.h>
#include "MrLCDmega32.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

static volatile uint16_t pulse_0 = 0;
static volatile unsigned char i_0 = 0;
static volatile uint16_t pulse_1 = 0;
static volatile unsigned char i_1 = 0;

int main(void)
{
	int16_t count_a = 0;
	int16_t count_b = 0;
	unsigned char show_a[4];
	Initialise();
	DDRD &= ~(1 << DD2) & ~(1 << DD3);
	DDRB |= (1 << DD0) | (1 << DD1);

	//_delay_ms(50);

	Initialise();

	GICR |= (1 << INT0) | (1 << INT1);
	MCUCR |= (1 << ISC00) | (1 << ISC10) ;

	sei();

	while(1)
	{
		PORTB |= (1 << PORT0);
		_delay_us(15);
		PORTB &= ~( 1<< PORT0);
		_delay_ms(50);
		count_a = pulse_0/58;

		PORTB |= (1 << PORT1);
		_delay_us(15);
		PORTB &= ~( 1<< PORT1);
		_delay_ms(50);
		count_b = pulse_1/58;

		/* Send_A_String("Distance Sensor");
		GoToMrLCDLocation(1,2); */
		Send_A_String("Distance1=");
		itoa(count_a,show_a,10);
		Send_A_String(show_a);
		Send_A_String(" ");
		GoToMrLCDLocation(15,1);
		Send_A_String("cm");
		GoToMrLCDLocation(1,2);
		Send_A_String("Distance2=");
		itoa(count_b,show_a,10);
		Send_A_String(show_a);
		Send_A_String(" ");
		GoToMrLCDLocation(15,2);
		Send_A_String("cm");
		GoToMrLCDLocation(1,1);
	}
}

ISR(INT0_vect)
{
	if(i_0 == 1)
	{
		TCCR1B = 0;
		pulse_0 = TCNT1;
		TCNT1 = 0;
		i_0 = 0;
	}

	if(i_0 == 0)
	{
		TCCR1B |= 1 << CS10;
		i_0 = 1;
	}
}

ISR(INT1_vect)
{
	if(i_1 == 1)
	{
		TCCR1B = 0;
		pulse_1 = TCNT1;
		TCNT1 = 0;
		i_1 = 0;
	}

	if(i_1 == 0)
	{
		TCCR1B |= 1 << CS10;
		i_1 = 1;
	}
}