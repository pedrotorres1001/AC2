#include <detpic32.h>

unsigned int value = 0;

void delay(unsigned int ms) {
        resetCoreTimer();
        while(readCoreTimer() < 20000 * ms);
}

void init() {
        TRISB = 0xBFFF;
        LATC = 0x0000;

        TRISD = 0xFF9F;
        LATD = 0x0000;

        TRISB = 0x80FF;

        T5CONbits.TCKPS = 2;    // K = 4    K = (PBCLK /(65536 * 100))
        PR5 = 49999;            // PR5 = ((20MHz / K) / 100 ) - 1 = 49999;  
        TMR5 = 0;               // Clear timer T2 count register
        T5CONbits.TON = 1;      // Enable timer T2 (must be the last command of the
                                // timer configuration sequence)
        IPC5bits.T5IP = 2;      // Interrupt priority (must be in range [1..6])
        IEC0bits.T5IE = 1;      // Enable timer T2 interrupts
        IFS0bits.T5IF = 0;      // Reset timer T2 interrupt flag

        TRISBbits.TRISB4 = 1;           // RB4 digital output disconnected
        AD1PCFGbits.PCFG4= 0;           // RB4 configured as analog input
        AD1CON1bits.SSRC = 0;           // Conversion trigger selection bits: in this
                                        // mode an internal counter ends sampling and
                                        // starts conversion
        AD1CON1bits.CLRASAM = 1;        // Stop conversions when the 1st A/D converter
                                        // interrupt is generated. At the same time,
                                        // hardware clears the ASAM bit
        AD1CON3bits.SAMC = 16;          // Sample time is 16 TAD (TAD = 100 ns)
        AD1CON2bits.SMPI = 0;           // Interrupt is generated after N samples
                                        // (replace N by the desired number of
                                        // consecutive samples)
        AD1CHSbits.CH0SA = 4;           // replace x by the desired input
                                        // analog channel (0 to 15)
        AD1CON1bits.ON = 1;             // Enable A/D converter
                                        // This must the last command of the A/D
                                        // configuration sequence

        IPC6bits.AD1IP = 1;             // configure priority of A/D interrupts
        IFS1bits.AD1IF = 0;             // clear A/D interrupt flag
        IEC1bits.AD1IE = 1;             // enable A/D interrupts
}

unsigned int toBcd(unsigned int value) {
        return ((value / 10) << 4) + (value % 10);
}

void setPWM(unsigned int dutycycle) {
        if(dutycycle >= 0 && dutycycle <= 100) {
                OC3RS = 200 * dutycycle;  // Ton constant
        }
}

void send2displays(unsigned int value) {
        static unsigned int displayFlag = 0;
        static unsigned int display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
        unsigned int digit_low = value & 0x0F;
        unsigned int digit_high = value >> 4;
        if(displayFlag == 0) {
                LATDbits.LATD6 = 1;             // Ativar o Segmento 1
                LATDbits.LATD5 = 0;
                LATB = (LATB & 0x80FF) | (display7Scodes[digit_high] << 8);
        } else {
                LATDbits.LATD6 = 0;             // Ativar o Segmento 0
                LATDbits.LATD5 = 1;
                LATB = (LATB & 0x80FF) | (display7Scodes[digit_low] << 8);
        }
        displayFlag = !displayFlag;
}

void _int_(27) isr_ADC(void) {
        value = (100 * ADC1BUF0 + 511) / 1023; // x100 pq queremos que apresente os valores no intervalo [0,100]
        setPWM(value);
        if(value == 100) {
                value = 0x00;
                LATC = 0x4000;
        } else {
                LATC = 0x0000;
        }
        delay(100);
        AD1CON1bits.ASAM = 1;   // iniciar a conversão
        IFS1bits.AD1IF = 0;     // Reset AD1IF flag
}

void _int_(20) isr_T5(void) {
        send2displays(toBcd(value));            // enviar o valor atual de value convertido em bcd para os displays valor compreendido entre [0,100]
        IFS0bits.T5IF = 0;                      // reset da flag do timer 5
}

int main(void) {
        init();  
        EnableInterrupts();
        AD1CON1bits.ASAM = 1;   // iniciar a conversão
        while(1);
        return 0;
}
