#include <avr/io.h>

int main(){
    DDRA = 0b00000000;
    PINA = 0b11111111;
}