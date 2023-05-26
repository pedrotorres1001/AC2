#include <detpic32.h>

void delay(unsigned int ms) {
    resetCoreTimer();
    while (readCoreTimer() < 20000 * ms);
}

void init() {
        T3CONbits.TCKPS = 0;    // 1:32 prescaler (i.e. fout_presc = 625 KHz)
        PR3 = 19999;            // PR3 = (20MHz / K) / 1000 = 19999;  
        TMR3 = 0;               // Clear timer T2 count register
        T3CONbits.TON = 1;      // Enable timer T2 (must be the last command of the
                                // timer configuration sequence)
        IPC3bits.T3IP = 2;      // Interrupt priority (must be in range [1..6])
        IEC0bits.T3IE = 1;      // Enable timer T2 interrupts
        IFS0bits.T3IF = 0;      // Reset timer T2 interrupt flag

        OC3CONbits.OCM = 6;     // PWM mode on OCx; fault pin disabled
        OC3CONbits.OCTSEL = 1;  // Use timer T3 as the time base for PWM generation
        OC3RS = 20000;          // Ton constant
        OC3CONbits.ON = 1;      // Enable OC3 module

        OC5CONbits.OCM = 6;     // PWM mode on OCx; fault pin disabled
        OC5CONbits.OCTSEL = 1;  // Use timer T3 as the time base for PWM generation
        OC5RS = 20000;          // Ton constant
        OC5CONbits.ON = 1;      // Enable OC5 module

        TRISBbits.TRISB4 = 1;   // RBx digital output disconnected
        AD1PCFGbits.PCFG4= 0;   // RBx configured as analog input
        AD1CON1bits.SSRC = 7;   // Conversion trigger selection bits: in this
                                // mode an internal counter ends sampling and
                                // starts conversion
        AD1CON1bits.CLRASAM = 1;// Stop conversions when the 1st A/D converter
                                // interrupt is generated. At the same time,
                                // hardware clears the ASAM bit
        AD1CON3bits.SAMC = 16;  // Sample time is 16 TAD (TAD = 100 ns)
        AD1CON2bits.SMPI = 0;   // Interrupt is generated after N samples
                                // (replace N by the desired number of
                                // consecutive samples)
        AD1CHSbits.CH0SA = 4;   // replace x by the desired input
                                // analog channel (0 to 15)
        AD1CON1bits.ON = 1;     // Enable A/D converter
                                // This must the last command of the A/D
                                // configuration sequence
}

void setPWM(unsigned int dutycycle) {
    if(dutycycle >= 0 && dutycycle <= 100) {
            OC3RS = 200 * dutycycle;  // Ton constant
            OC5RS = 200 * (100-dutycycle);
    }
}

int main(void) {
    init();
    unsigned int value = 0;
        
    while(1) {
        AD1CON1bits.ASAM = 1;
        while (IFS1bits.AD1IF == 0);
        value = (100 * ADC1BUF0 + 511) / 1023;
        setPWM(value);
        IFS1bits.AD1IF = 0;
        delay(100);
    }
}

