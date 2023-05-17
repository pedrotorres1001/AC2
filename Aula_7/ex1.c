#include <detpic32.h>

int main(void) {

    //Configure outputs and stuff
    TRISBbits.TRISB4 = 1;    // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0;   // RB4 configured as analog input (AN4)
    AD1CON1bits.SSRC = 7;

    AD1CON2bits.SMPI = 0;    // 1 sample will be converted and stored in buffer locations ADC1BUF0  
    AD1CHSbits.CH0SA = 4;    // Selects AN4 as input for the A/D converter
    AD1CON1bits.ON = 1;      // Enable A/D converter

    // Configure interrupt system
    IPC6bits.AD1IP = 2; // configure priority of A/D interrupts
    IFS1bits.AD1IF = 0; // clear A/D interrupt flag
    IEC1bits.AD1IE = 1; // enable A/D interrupts
    EnableInterrupts(); // Global Interrupt Enable

    // Start A/D conversion
    AD1CON1bits.ASAM = 1; // Start conversion

    while(1); // all activity is done by the ISR
    return 0;
}

// Interrupt Handler
void _int_(27) isr_adc(void) {
    // Read conversion result (ADC1BUF0) and print it
    printInt(ADC1BUF0, 10 | 3 << 8); // Read conversion result (ADC1BUF0 value) and print it
    putChar(' ');
    // Start A/D conversion
    AD1CON1bits.ASAM = 1; // Start conversion
    IFS1bits.AD1IF = 0; // Reset AD1IF flag
}
