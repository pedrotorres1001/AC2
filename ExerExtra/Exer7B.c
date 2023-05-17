#include <detpic32.h>
#define msVal 20000

void delay(int ms);

int main(void) {

    // Configure all (digital I/O, analog input, A/D module)
    
    TRISBbits.TRISB4 = 1; // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0; // RB4 configured as analog input (AN
    AD1CON1bits.SSRC = 7; // Conversion trigger selection bits: in this
    
    AD1CON2bits.SMPI = 0;
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
    
    IPC6bits.AD1IP = 2;
    IFS1bits.AD1IF = 0; 
    IEC1bits.AD1IE = 1;

    EnableInterrupts(); // Global Interrupt Enable
    // Start A/D conversion
    AD1CON1bits.ASAM = 1; // Start conversion

    while (1) {
    }
    return 0;
}

// Interrupt Handler
void _int_(27) isr_adc(void) // Replace VECTOR by the A/D vector
// number - see "PIC32 family data
// sheet" (pages 74-76)
{
    char bit;
    int final, i;
    // Read conversion result (ADC1BUF0) and print it in binary format
    for (i = 15; i >= 0; i--) {
        bit = (ADC1BUF0 >> i) & 1; // extract each bit from the result
        if (i == 16) {
            printStr("DS16=(");
        }
        else if (i == 15) {
            printStr("DS15=(");
        }
        else if (i == 14) {
            printStr("DS14=(");
        }
        else if (i == 13) {
            printStr("DS13=(");
        }
        else if (i == 12) {
            printStr("DS12=(");
        }
        else if (i == 11) {
            printStr("DS11=(");
        }
        else if (i == 10) {
            printStr("DS10=(");
        }
        else if (i == 9) {
            printStr("DS9=(");
        }
        else if (i == 8) {
            printStr("DS8=(");
        }
        else if (i == 7) {
            printStr("DS7=(");
        }
        else if (i == 6) {
            printStr("DS6=(");
        }
        else if (i == 5) {
            printStr("DS5=(");
        }
        else if (i == 4) {
            printStr("DS4=(");
        }
        else if (i == 3) {
            printStr("DS3=(");
        }
        else if (i == 2) {
            printStr("DS2=(");
        }
        else {
            printStr("DS1=(");
        }
        putChar(bit + '0'); // print the bit as a character ('0' or '1')
        printStr("), ");
    }
    putChar('\n');
    putChar('\r');
    // Start A/D conversion
    IPC6bits.AD1IP = 2; // configure priority of A/D interrupts
    IFS1bits.AD1IF = 0; // Reset AD1IF flag
}