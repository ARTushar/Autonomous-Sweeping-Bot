#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

volatile int global_count;

ISR(INT1_vect)
{
	global_count++;
}

int main(void)
{
    DDRD |= 0xF0;
    DDRC |= 0xC0;

	GICR = (1 << INT1);
	MCUCR = (1 << ISC10) | (1 << ISC11);
	sei(); 
	
	Lcd4_Init();
    Lcd4_Set_Cursor(1,5);
	
	int count;
	char rotation[5];
	rotation[4] = '\0';
	int i=0;
	for(i=0; i<4; i++) {
		rotation[i] = '0';
	}
	
	while(1)
	{
		count = global_count;
		i = 3;
		
		while(count) {
			rotation[i] = count%10 + '0';
			count = count/10;
			i--;
		}
	
		Lcd4_Set_Cursor(1,2);
		Lcd4_Write_String("Rotations");
		Lcd4_Set_Cursor(2,4);
		Lcd4_Write_String(rotation);
		_delay_ms(500);
		Lcd4_Clear();
	}
}
