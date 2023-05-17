#include <detpic32.h>
int main(void)
{
    TRISDbits.TRISD11 = 0; // RD11 digital output disconnected
    LATDbits.LATD11 = 0; // Reset RD11 (LATD11 = 0)

    // Configure all (digital I/O, analog input, A/D module)
    TRISBbits.TRISB4 = 1; // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0; // RB4 configured as analog input (AN
    AD1CON1bits.SSRC = 7; // Conversion trigger selection bits: in this


    AD1CON1bits.CLRASAM = 1; // Stop conversions when the 1st A/D converter
    AD1CON3bits.SAMC = 16; // Sample time is 16 TAD (TAD = 100 ns)


    AD1CON2bits.SMPI = 0;
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
    
    IPC6bits.AD1IP = 2;
    IFS1bits.AD1IF = 0; 
    IEC1bits.AD1IE = 1;

    EnableInterrupts(); // Global Interrupt Enable
    // Start A/D conversion
    AD1CON1bits.ASAM = 1; // Start conversion
    
    while(1)
    {
        // Reset RD11 (LATD11 = 0)
        LATDbits.LATD11 = 0;
    }
    return 0;
}

// Interrupt Handler
void _int_(27) isr_adc(void) // Replace VECTOR by the A/D vector
// number - see "PIC32 family data
// sheet" (pages 74-76)
{
    volatile int adc_value;
    // Reset RD11 (LATD11 = 0)
    LATDbits.LATD11 = 0;
    // Read ADC1BUF0 value to "adc_value"
    adc_value = ADC1BUF0;
    // Start A/D conversion
    AD1CON1bits.ASAM = 1; // Start conversion
    IFS1bits.AD1IF = 0; // Reset AD1IF flag
    LATDbits.LATD11 = 1;
}
