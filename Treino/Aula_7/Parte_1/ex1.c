#include <detpic32.h>

int main(void){

    TRISBbits.TRISB4 = 1;
    AD1PCFGbits.PCFG4 = 0;
    AD1CON1bits.SSRC = 7;

    AD1CON2bits.SMPI = 0;
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;

    IPC6bits.AD1IP = 2;
    IFS1bits.AD1IF = 0;
    IEC1bits.AD1IE = 1;

    EnableInterrupts();
    AD1CON1bits.ASAM = 1;

    while(1);
    return 0;
}

void _int_(27) isr_adc(void){
    printInt(ADC1BUF0, 10 | 4 << 16);
    putChar(' ');
    AD1CON1bits.ASAM = 1;
    IFS1bits.AD1IF = 0;
}
