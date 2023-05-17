#include <detpic32.h>
#define msVal 20000

void delay(unsigned int);

void send2displays(unsigned char value)
{
    static const char display7Scodes[] = {0x3F, 0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
    // select display high
    // send digit_high (dh) to display: dh = value >> 4
    // select display low
    // send digit_low (dl) to display: dl = value & 0x0F
    LATDbits.LATD5 = 1;
    LATDbits.LATD6 = 0;
    LATB = (LATB & 0x80FF) | (display7Scodes[value & 0x0F] << 8);
    delay(100);
    LATDbits.LATD5 = 0;
    LATDbits.LATD6 = 1;
    LATB = (LATB & 0x80FF) | (display7Scodes[value >> 4] << 8);
}

int main(void) {

    TRISE = 0xFFF0; // configure RE0-RE3 as outputs
    TRISB = 0x80FF; // configure RB8-RB14 as outputs
    TRISD = 0xFF9F; // 1111 1111 1001 1111

    char c;

    while (1) {
        c = getChar();
        if (c == '0') {
            PORTE = 0x0001;
            send2displays(0x00);
        } else if (c == '1') {
            PORTE = 0x0002;
            send2displays(0x01);       
        } else if (c == '2') {
            PORTE = 0x0004;
            send2displays(0x02);
        } else if (c == '3') {
            PORTE = 0x0008;
            send2displays(0x03);
        } else {
            PORTE = 0x000F;
            send2displays(0xFF);
            delay(1000);
            PORTE = 0X0000;
            send2displays(0x00);
        }
    }
    return 0;
}   

void delay (unsigned int ms) {
    unsigned int wait;

    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}