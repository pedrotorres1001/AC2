#include <detpic32.h>

volatile int duty_cycle = 25;

void delay (unsigned int ms) {
    resetCoreTimer();
    while (readCoreTimer() < 20000 * (ms / 1000));
}

void init() {
    TRISB = TRISB | 0x000F;
    T2CONbits.TCKPS = 2; // 1:4 prescaler 
    PR2 = 33332; // Fout = 20 MHz / (4 * (33332 + 1)) = 150 Hz
    TMR2 = 0;
    T2CONbits.TON = 1;

    OC2CONbits.OCM = 6;
    OC2CONbits.OCTSEL = 0;
    OC2RS = (PR2 * duty_cycle) / 100;
    OC2CONbits.ON = 1;
} 

int duty_cycle() {
    if (PORTBbits.RB3 == 0 && PORTBbits.RB0 == 1) {
        duty_cycle = 25; 
    } else if (PORTBbits.RB3 == 1 && PORTBbits.RB0 == 0) {
        duty_cycle = 70;
    }
    return duty_cycle;
}

int main (void) {
    init();
    delay(250);
    while(1);
    return 0;
}