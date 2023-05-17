#include <detpic32.h>
#define msVal 20000

volatile int voltage = 0; // Global variable

void delay (unsigned int ms) {
    unsigned int wait;
    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

void send2displays(unsigned char value) {

    static int displayFlag = 0;
    static const char display7Scodes[] = {0x3F, 0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

    if (displayFlag == 0) {
        // select display high
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;
        // send digit_high (dh) to display: dh = value >> 4
        LATB = (LATB & 0x80FF) | (display7Scodes[value >> 4] << 8);
        displayFlag = 1;
    } else {
        // select display low
        LATDbits.LATD5 = 1;
        LATDbits.LATD6 = 0;
        // send digit_low (dl) to display: dl = value & 0x0F
        LATB = (LATB & 0x80FF) | (display7Scodes[value & 0x0F] << 8);
        displayFlag = 0;
    }
}

int main(void) {

    T3CONbits.TCKPS = 5; // 1:32 prescaler (i.e. fout_presc = 625 KHz)
    PR3 = 6249; // Fout = 20MHz / (32 * (6249 + 1)) = 100 Hz
    TMR3 = 0; // Clear timer T3 count register
    T3CONbits.TON = 1; // Enable timer T3 (must be the last command of the timer configuration sequence)

    IPC3bits.T3IP = 2; // configure priority of interrupts
    IEC0bits.T3IE = 1; // enable T3 interrupts



    T1CONbits.TCKPS = 6; // 1:64 prescaler (i.e. fout_presc = 625 KHz)
    PR1 = 62499; // Fout = 20MHz / (32 * (62499 + 1)) = 5 Hz
    TMR1 = 0; // Clear timer T3 count register
    T1CONbits.TON = 1; // Enable timer T3 (must be the last command of the timer configuration sequence)

    IPC1bits.T1IP = 2; // configure priority of interrupts
    IEC0bits.T1IE = 1; // enable T3 interrupts

    EnableInterrupts(); // Macro defined in "detpic32.h"

    TRISBbits.TRISB4 = 1;    // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0;   // RB4 configured as analog input (AN4)
    AD1CON1bits.SSRC = 7;    
    
    // Configure interrupt system
    IPC6bits.AD1IP = 2; // configure priority of A/D interrupts
    IFS1bits.AD1IF = 0; // clear A/D interrupt flag
    IEC1bits.AD1IE = 1; // enable A/D interrupts

    AD1CON2bits.SMPI = 7;    // 1 sample will be converted and stored in buffer locations ADC1BUF0  
    AD1CHSbits.CH0SA = 4;    // Selects AN4 as input for the A/D converter
    AD1CON1bits.ON = 1;      // Enable A/D converter

    TRISB = 0x80FF;
    TRISD = 0xFF9F;
    
    while (1);
    return 0;
}

void _int_(4) isr_T1(void) {
    
    AD1CON1bits.ASAM = 1;
    IFS0bits.T1IF = 0;

}

void _int_(12) isr_T3(void) {

    send2displays(toBcd(voltage));
    IFS0bits.T3IF = 0;

}

void _int_(27) isr_adc(void) {

    int *p = (int *)(&ADC1BUF0);
    int i, media = 0;
    for (i = 0; i < 8; i++) {
        media += p[i*4];
    }

    media = media / 8;
    int v = (media * 33 + 511) / 1023;

    voltage = v;

    IFS1bits.AD1IF = 0;

}
