#include <detpic32.h>

volatile int count = 0;

unsigned int toBcd(unsigned int value) {
    return ((value / 10) << 4) + (value % 10);
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

void putc(char byte){
    // wait while UTXBF == 1
    while(U2STAbits.UTXBF == 1);
    // Copy byte to the UxTXREG register
    U2TXREG = byte;
}

void init() {
    T1CONbits.TCKPS = 5;    // K = 4    K = (PBCLK /(65536 * 100))
    PR1 = 62499;            // PR5 = ((20MHz / K) / 100 ) - 1 = 49999;  
    TMR1= 0;               // Clear timer T2 count register
    T1CONbits.TON = 1;      // Enable timer T2 (must be the last command of the
                            // timer configuration sequence)
    IPC1bits.T1IP = 2;      // Interrupt priority (must be in range [1..6])
    IEC0bits.T1IE = 1;      // Enable timer T1 interrupts
    IFS0bits.T1IF = 0;      // Reset timer T1 interrupt flag

    T2CONbits.TCKPS = 3;    // K = 4    K = (PBCLK /(65536 * 100))
    PR2 = 49999;            // PR5 = ((20MHz / K) / 100 ) - 1 = 49999;  
    TMR2 = 0;               // Clear timer T2 count register
    T2CONbits.TON = 1;      // Enable timer T2 (must be the last command of the
                            // timer configuration sequence)
    IPC2bits.T2IP = 2;      // Interrupt priority (must be in range [1..6])
    IEC0bits.T2IE = 1;      // Enable timer T1 interrupts
    IFS0bits.T2IF = 0;      // Reset timer T1 interrupt flag

    TRISD = 0xFF9F;
    TRISB = 0x80FF;

    EnableInterrupts();
}

int main(void) {
    init();
    while(1);
    return 0;
}

void _int_(4) isr_T1(void) {
    count++;
    if (count == 100) {
        count = 0;
    } else {
        printInt(count, 16 | 2 << 16);
        putc('\n');
    }
    IFS0bits.T1IF = 0;
}

void _int_(8) isr_T2(void) {
    send2displays(toBcd(count));
    IFS0bits.T2IF = 0;
}
