#ifndef motors
#define motors

#include <avr/io.h>

#define leftMotorPWMPin		OCR1B
#define rightMotorPWMPin	OCR1A

void init_motors(void);
void set_motors(int leftMotorSpeed, int rightMotorSpeed);


/*	Initializes the OC1A and OC1B pins for 8-bit phase correct PWM.
 *	with non-inverting mode. OC1A/OC1B is cleared on compare match 
 *	when up-counting and is set on compare match when down-counting.
 */
void init_motors()
{
	//Configure PWM pins OC1B and OC1A to output mode
    DDRD |= (1<<PIND4) | (1<<PIND5);
	//Configure motor direction control pins to output mode
	DDRD |= (1<<PIND6) | (1<<PIND7);
	//Clear OC1A/OC1B on compare match when up-counting and set OC1A/OC1B 
	//on compare match when downcounting (sets PWM to non-inverting mode)
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1);	
	//Selects prescalar value 64
	TCCR1B |= (1<<CS10) | (1<<CS11);
	//Phase Correct PWM mode is selected
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM13);
	//Sets TOP value to be 250. Frequency is 500Hz
	ICR1 = 250;
}


/*	Sets the speed and direction of the two motors. The value of each
 *	of the arguments can range from -250 to +250. A negative sign
 *	is used when the direction of the motor is to be reversed.
 */
void set_motors(int leftMotorSpeed, int rightMotorSpeed)
{
    if(leftMotorSpeed >= 0)
    {
        leftMotorPWMPin = leftMotorSpeed;
		PORTD &= ~(1<<PIND7);
    }
    else
    {
        leftMotorPWMPin = 250 + leftMotorSpeed;
        PORTD |= 1<<PIND7;
    }
	if(rightMotorSpeed >= 0)
    {
        rightMotorPWMPin = rightMotorSpeed;
        PORTD &= ~(1<<PIND6);
    }
    else
    {
        rightMotorPWMPin = 250 + rightMotorSpeed;
        PORTD |= 1<<PIND6;
    }
}

#endif