
#ifndef F_CPU
#define F_CPU 1000000UL // 1 MHz clock speed
#endif

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

volatile uint32_t main_condition_counter = 0;
volatile uint16_t total_counter = 0;
volatile uint8_t traverseCompleted = 0;

int main(void)
{
    uint16_t result = 0;
    uint16_t temp = 0;
    uint32_t condition_counter = 0;
    uint16_t counter = 0;
    DDRD |= 0xF0;
    DDRC |= 0xC0;
    DDRA |= 0x04;

    DDRD &= ~(1 << DD2);

    Lcd4_Init();
    
    /** using interrput 0 for showing the final condition
     * of the air quiality. When the traversal of bot is completed
     * the interrupt pin will be high
     */
    GICR |= (1 << INT0);
    MCUCR |= (1 << ISC00) | (1 << ISC01);
    sei();

    ADMUX = 0b01000000;
    ADCSRA = 0b10000010;
    PORTA |= (1 << PORTA2);
    Lcd4_Clear();

    while (!traverseCompleted)
    {
        PORTA &= ~(1 << PORTA2);
        _delay_us(280);

        ADCSRA |= (1 << ADSC);
        _delay_us(40);
        PORTA |= (1 << PORTA2);
        _delay_us(9680);
        while (ADCSRA & (1 << ADSC))
        {
        }
        result = ADCL;
        temp = ADCH;
        result |= (temp << 8);
        float res = (result * 5.0 / 1024);
        float dustDensity;
        if (res < 0.583)
            dustDensity = 0;
        else
            dustDensity = 0.17 * res - 0.1;
        // res *= 100;
        /* unsigned char final_res = (int) res;
        unsigned char final_result[5];
        final_result[0] = final_res / 100 + '0';
        final_result[1]= '.';
        final_result[3] = (final_res % 10) + '0' ;
        final_result[2] = ((final_res/10) % 10) + '0';
        final_result[4] = '\0';

        // Lcd4_Set_Cursor(1,1);
        // Lcd4_Write_String("Voltage : ");
        // Lcd4_Write_String(final_result); */

        if (dustDensity <= 0.04)
            condition_counter += 0;
        else if (dustDensity <= 0.08)
            condition_counter += 1;
        else if (dustDensity <= 0.16)
            condition_counter += 5;
        else
            condition_counter += 10;
        counter = (counter + 1) % 101;

        if (counter == 100) {
            uint16_t temp = condition_counter / 100;
            Lcd4_Clear();
            Lcd4_Set_Cursor(2, 0);
            Lcd4_Write_String("Air Q:");
            if(temp == 0) {
                Lcd4_Write_String("excellent");
            } else if(temp <= 1) {
                Lcd4_Write_String("good :)");
            } else if(temp <=2){
                Lcd4_Write_String("poor!");
            } else {
                Lcd4_Write_String("worse :(");
            }
            main_condition_counter += temp;
            condition_counter = 0;
            total_counter++;
        }

        dustDensity *= 100;
        unsigned char dust_res = (int)dustDensity;
        unsigned char dust_result[5];
        dust_result[0] = dust_res / 100 + '0';
        dust_result[1] = '.';
        dust_result[3] = (dust_res % 10) + '0';
        dust_result[2] = ((dust_res / 10) % 10) + '0';
        dust_result[4] = '\0';

        Lcd4_Set_Cursor(1, 0);
        Lcd4_Write_String("Density : ");
        Lcd4_Write_String(dust_result);
    }
}
ISR(INT0_vect){
    main_condition_counter = main_condition_counter/(total_counter + 1);
    Lcd4_Set_Cursor(2, 0);
    Lcd4_Write_String("Air Q:");
    if(main_condition_counter == 0) {
        Lcd4_Write_String("excellent!");
    } else if(main_condition_counter <= 2) {
        Lcd4_Write_String("good :)");
    } else if(main_condition_counter <=3){
        Lcd4_Write_String("poor :|");
    } else {
        Lcd4_Write_String("worse :(");
    }
    traverseCompleted = 1;
}