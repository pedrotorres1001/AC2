#include <detpic32.h>
#define msVal 20000

volatile int voltage = 0; // Global variable
volatile int voltMin = 0;
volatile int voltMax = 0;


void delay (unsigned int ms) {
    unsigned int wait;
    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}

void putc(char byte)
{
    // wait while UART2 UTXBF == 1
    while(U2STAbits.UTXBF == 1);
    // Copy "byte" to the U2TXREG register
    U2TXREG = byte;
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
    // Configure UART2:
    // Configure BaudRate Generator
    U2BRG = (((20000000+(8*115200))/(16*115200)) - 1); // 115200 bps para transmissão/receção 
    U2MODEbits.BRGH = 0; // fator de 16
    // Configure number of data bits, parity and number of stop bits
    U2MODEbits.PDSEL1 = 0;
    U2MODEbits.PDSEL0 = 0;
    U2MODEbits.STSEL = 1;
    // Enable the trasmitter and receiver modules (see register U2STA)
    U2STAbits.UTXEN = 1;
    U2STAbits.URXEN = 1;
    // Enable UART2 (see register U2MODE)
    U2MODEbits.ON = 1;

    // Configure UART2 interrupts, with RX interrupts enabled
    //and TX interrupts disabled:
    //enable U2RXIE, disable U2TXIE (register IEC1)
    //set UART2 priority level (register IPC8)
    //clear Interrupt Flag bit U2RXIF (register IFS1)
    //define RX interrupt mode (URXISEL bits)

    IEC1bits.U2RXIE = 1;
    IEC1bits.U2TXIE = 0;

    IPC8bits.U2IP = 1;
    IPC8bits.U2IS = 2;

    IFS1bits.U2RXIF = 0;

    U2STAbits.URXISEL1 = 0;
    U2STAbits.URXISEL0 = 0;

    // ######################################

    TRISB = 0x80FF;
    TRISD = 0xFF9F;

    TRISBbits.TRISB4 = 1;    // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0;   // RB4 configured as analog input (AN4)
    AD1CON1bits.SSRC = 7;    
    
    AD1CON2bits.SMPI = 7;    // 1 sample will be converted and stored in buffer locations ADC1BUF0  
    AD1CHSbits.CH0SA = 4;    // Selects AN4 as input for the A/D converter

    // Configure interrupt system
    IPC6bits.AD1IP = 2; // configure priority of A/D interrupts
    IFS1bits.AD1IF = 0; // clear A/D interrupt flag
    IEC1bits.AD1IE = 1; // enable A/D interrupts

    AD1CON1bits.ON = 1;      // Enable A/D converter

    T3CONbits.TCKPS = 5; // 1:32 prescaler (i.e. fout_presc = 625 KHz)
    PR3 = 6249; // Fout = 20MHz / (32 * (312499 + 1)) = 2 Hz
    TMR3 = 0; // Clear timer T3 count register
    T3CONbits.TON = 1; // Enable timer T3 (must be the last command of the timer configuration sequence)

    IPC3bits.T3IP = 2; // configure priority of interrupts
    IEC0bits.T3IE = 1; // enable T3 interrupts

    T1CONbits.TCKPS = 6; // 1:32 prescaler (i.e. fout_presc = 625 KHz)
    PR1 = 62499; // Fout = 20MHz / (32 * (312499 + 1)) = 2 Hz
    TMR1 = 0; // Clear timer T3 count register
    T1CONbits.TON = 1; // Enable timer T3 (must be the last command of the timer configuration sequence)

    IPC1bits.T1IP = 2; // configure priority of interrupts
    IEC0bits.T1IE = 1; // enable T3 interrupts

    // reset AD1IF flag, T3IF flag, T1IF flag

    IFS1bits.AD1IF = 0;      // Reset AD1IF flag
    IFS0bits.T3IF = 0;       // Reset T3IF flag
    IFS0bits.T1IF = 0;       // Reset T1IF flag

    EnableInterrupts(); // Macro defined in "detpic32.h"
    
    while (1);
    return 0;
}

void _int_(4) isr_T1(void) {
    
    AD1CON1bits.ASAM = 1;
    IFS0bits.T1IF = 0;

}

void _int_(12) isr_T3(void) {
    
    send2displays(voltage);
    IFS0bits.T3IF = 0;

}

void _int_(27) isr_adc(void) {

    int *p = (int *)(&ADC1BUF0);
    int i, media = 0;
    for (i = 0; i < 8; i++) {
        media += p[i*4];
    }

    media = media / 8;
    int V = (media * 33 + 511) / 1023;

    voltage = toBcd(V);

    // Update variables "voltMin" with the lowest voltage at the start and "voltMax" with the highest voltage at the start
    if (voltage < voltMin) {
        voltMin = voltage;
    }
    if (voltage > voltMax) {
        voltMax = voltage;
    }
    // Reset AD1IF flag
    IFS1bits.AD1IF = 0;
}

void _int_(32) isr_uart2(void)
{
    // Read character from FIFO
    char c = U2RXREG;
    if(c == 'M') {
        // Send "voltMax" to the serial port UART2
        putc((voltMax >> 4) + 0x30);
        putc('.');
        putc((voltMax & 0x0F) + 0x30);
        putc('V');
        putc('\n');
    }else if(c == 'm') {
        // Send "voltMin" to the serial port UART2
        putc((voltMin >> 4) + 0x30);
        putc('.');
        putc((voltMin & 0x0F) + 0x30);
        putc('V');
        putc('\n');
    }
    // Clear UART2 rx interrupt flag
    IFS1bits.U2RXIF = 0;
}