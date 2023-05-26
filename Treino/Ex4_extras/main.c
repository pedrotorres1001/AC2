#include <detpic32.h>

unsigned int value = 0;

void delay(unsigned int ms){
    resetCoreTimer();
    while(readCoreTimer() < 20000*ms);
}

void send2displays(unsigned char value) {
    static const char display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    static char displayFlag = 0;

    int digit_low = value & 0x0F;
    int digit_high = value >> 4;

    if (displayFlag == 0) {
        LATDbits.LATD5 = 1; 
        LATDbits.LATD6 = 0;
        LATB = (LATB & 0x00FF) | (display7Scodes[digit_low] << 8);
    } else {
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;
        LATB = (LATB & 0x00FF) | (display7Scodes[digit_high] << 8);
    }
    displayFlag = !displayFlag;
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

int main(void) {
    TRISBbits.TRISB4 = 1; // RB4 digital output disconnected
    AD1PCFGbits.PCFG4= 0; // RB4 configured as analog input
    AD1CON1bits.SSRC = 7; // Conversion trigger selection bits: in this
    // mode an internal counter ends sampling and
    // starts conversion
    AD1CON1bits.CLRASAM = 1; // Stop conversions when the 1st A/D converter
    // interrupt is generated. At the same time,
    // hardware clears the ASAM bit
    AD1CON3bits.SAMC = 16; // Sample time is 16 TAD (TAD = 100 ns)
    AD1CON2bits.SMPI = 0; // Interrupt is generated after 8 samples
    // (replace N by the desired number of
    // consecutive samples)
    AD1CHSbits.CH0SA = 4; 
    // analog channel (0 to 15)
    AD1CON1bits.ON = 1; // Enable A/D converter
    // This must the last command of the A/D
    // configuration sequence 

    T5CONbits.TCKPS = 2; // 20*10^6 / 65536 * 100 = 3
    PR5 = 49999; // Fout = 20MHz /4 = / 100  =  50000
    TMR5 = 0; // Clear timer T3 count register
    T5CONbits.TON = 1; // Enable timer T5 (must be the last command of the timer configuration sequence)
    
    IPC5bits.T5IP = 2; // Interrupt priority (must be in range [1..6])
    IEC0bits.T5IE = 1; // Enable timer T5 interrupts
    IFS0bits.T5IF = 0;      // Reset timer T5 interrupt flag


    IPC6bits.AD1IP = 1;             // configure priority of A/D interrupts
    IFS1bits.AD1IF = 0;             // clear A/D interrupt flag
    IEC1bits.AD1IE = 1;             // enable A/D interrupts

    TRISD = TRISD & 0xFF9F; // 1111 1111 1001 1111
    LATD = LATD & 0XFF9F;   // 1111 1111 1001 1111

    TRISB = TRISB & 0x80FF; // 1000 0000 1111 1111
    LATB = LATB & 0x80FF;   // 1000 0000 1111 1111

    TRISC = TRISC & 0xBFFF; // 1011 1111 1111 1111 
    LATC = LATC & 0xBFFF;   // 1011 1111 1111 1111 
    EnableInterrupts();
    AD1CON1bits.ASAM = 1; // Start conversion

    while(1);
    return 0;

}

void setPWM(unsigned int dutyCycle)
{
    // duty_cycle must be in the range [0, 100]
    if (dutyCycle >= 0 && dutyCycle <= 100)
    {
        OC3RS = ((PR5 + 1) * dutyCycle) / 100;
    }
} 

void _int_ (27) isr_ADC(void) {
    value = (100 * ADC1BUF0 + 511) / 1023;
    setPWM(value);

    if(value == 100) {
        value = 0x00;
        LATC = 0x4000; // 0100 0000 0000 0000
    } else {
        LATC = 0x0000; // 0000 0000 0000 0000
    }
    delay(100);
    AD1CON1bits.ASAM = 1; // Start conversion
    IFS1bits.AD1IF = 0; // Reset AD1IF
}

void _int_(20) isr_T5(void) {
    send2displays(toBcd(value));
    IFS0bits.T5IF = 0;      // Reset timer T5 interrupt flag
}

