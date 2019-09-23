#include "pwm_generation.h"
volatile int left_speed = 0;
volatile int right_speed = 0;

void init_motor_pins(){
    DDRA = 0x0F;
    init_pwm0_phase_mode_non_inverted();
    init_pwm2_phase_mode_non_inverted();
}
void set_speed(unsigned char left_wheel, unsigned char right_wheel) {
    left_speed = left_wheel;
    right_speed = right_wheel;

    if(left_wheel >= 0) {
        PORTA |= (1 << PORTA0);
        PORTA &= ~(1 << PORTA1);
        set_duty0_non_inverted(left_wheel);
    }
    else {
        PORTA |= (1 << PORTA1);
        PORTA &= ~(1 << PORTA0);
        set_duty0_non_inverted(left_wheel);
    }
    if(right_wheel >= 0) {
        PORTA |= (1 << PORTA2);
        PORTA &= ~(1 << PORTA3);
        set_duty2_non_inverted(right_wheel);
    }
    else {
        PORTA |= (1 << PORTA3);
        PORTA &= ~(1 << PORTA2);
        set_duty2_non_inverted(right_wheel);
        
    }
}

int get_left_speed(){
    return left_speed;
}

int get_right_speed(){
    return right_speed;
}

void stop_left_motor(){
    left_speed = 0;
    PORTA &= (~(1 << PORTA0) & ~(1 << PORTA1)); 
}
void stop_right_motor(){
    right_speed = 0;
    PORTA &= (~(1 << PORTA3) & ~(1 << PORTA2));
}
void rotate_right_90_fast(){
    set_speed(100, -100);
    _delay_ms(1000);
    set_speed(100, 100);
}
void rotate_left_90_fast(){
    set_speed(-100, 100);
    _delay_ms(1000);
    set_speed(100, 100);
}

void rotate_right_90(){
    set_speed(100, 0);
    _delay_ms(1200);
    set_speed(100, 100);
}
void rotate_left_90(){
    set_speed(0, 100);
    _delay_ms(1200);
    set_speed(100, 100);
}