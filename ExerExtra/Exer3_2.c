#include <detpic32.h>

#define msVal 20000

void delay(unsigned int);

int main(void) {

    TRISB = 0x000F; // configure RB0-RB3 as inputs
    TRISE = 0xFFF0; // configure RE0-RE3 as outputs
    char c;

    while (1) {
        c = getChar();
        if (c == '0') {
            PORTE = 0x0001;
        } else if (c == '1') {
            PORTE = 0x0002;
        } else if (c == '2') {
            PORTE = 0x0004;
        } else if (c == '3') {
            PORTE = 0x0008;
        } else {
            PORTE = 0x000F;
            delay(1000);
            PORTE = 0x0000;
        }
    }
    
    return 0;
}

void delay (unsigned int ms)
{
    unsigned int wait;

    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}