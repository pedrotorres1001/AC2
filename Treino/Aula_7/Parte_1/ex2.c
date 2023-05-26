#include <detpic32.h>

volatile unsigned char voltage = 0;

void delay(unsigned int ms){
    resetCoreTimer();
    while(readCoreTimer() < 20000 * ms);
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

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}


int main(void){

    unsigned int cnt = 0;

    TRISBbits.TRISB4 = 1;
    AD1PCFGbits.PCFG4 = 0;
    AD1CON1bits.SSRC = 7;

    AD1CON2bits.SMPI = 7;
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;

    IPC6bits.AD1IP = 2;
    IFS1bits.AD1IF = 0;
    IEC1bits.AD1IE = 1;

    EnableInterrupts();
    AD1CON1bits.ASAM = 1;

    while(1){
        if (cnt == 0){
            AD1CON1bits.ASAM = 1;
            send2displays(toBcd(voltage));
            cnt = (cnt + 1) % 25;
            delay(10);
        }
    }
    return 0;
}

void _int_(27) isr_adc(void){
    
    int *p;
    int i;
    int sum = 0;

    for (i = 0; i < 8, i++){
        sum += p[i*4];
    }
    int average = sum / 8;

    voltage = (average * 33 + 511) / 1023;

    AD1CON1bits.ASAM = 1;
    IFS1bits.AD1IF = 0;
}


// 5 hz = 200 ms 
// 100 hz = 10 ms