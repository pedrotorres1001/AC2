#include <detpic32.h>

void init() {
    T3CONbits.TCKPS = 3;    // 1:32 prescaler (i.e. fout_presc = 625 KHz)
    PR3 = 49999;            // Fout = 20MHz / (32 * (62499 + 1)) = 10 Hz
    TMR3 = 0;               // Clear timer T2 count register
    T3CONbits.TON = 1;      // Enable timer T2 (must be the last command of the
                            // timer configuration sequence

    T1CONbits.TCKPS = 6;
    PR1 = 62499;
    TMR1 = 0;
    T1CONbits.TON = 1;

    IPC3bits.T3IP = 2;      // Interrupt priority (must be in range [1..6])
    IEC0bits.T3IE = 1;      // Enable timer T3 interrupts

    IPC1bits.T1IP = 2;
    IEC0bits.T1IE = 1;
}

int main(void){
    init();
    IFS0bits.T1IF = 0;
    IFS0bits.T3IF = 0;
    EnableInterrupts();
    while(1);
    return 0;
}

void _int_(12) isr_T3(void) {
    putChar('3');
    IFS0bits.T3IF = 0;
}

void _int_(4) isr_T1(void){
    putChar('1');
    IFS0bits.T1IF = 0;
}




// k_pres = 1,2,4,8,16,32,64,256
//          0,1,2,3,4, 5, 6 ,7 