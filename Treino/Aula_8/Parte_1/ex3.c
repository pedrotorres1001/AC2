#include <detpic32.h>

void init() {
    T3CONbits.TCKPS = 7;    // 1:32 prescaler (i.e. fout_presc = 625 KHz)
    PR3 = 39061;            // Fout = 20MHz / (32 * (62499 + 1)) = 10 Hz
    TMR3 = 0;               // Clear timer T2 count register
    T3CONbits.TON = 1;      // Enable timer T2 (must be the last command of the
                            // timer configuration sequence

    IPC3bits.T3IP = 2;      // Interrupt priority (must be in range [1..6])
    IEC0bits.T3IE = 1;      // Enable timer T3 interrupts
}

int main(void){
    init();
    EnableInterrupts();
    while(1);
    return 0;
}

void _int_(12) isr_T3(void) {
    static int i = 0;
    i++;
    if (i == 2){
        putChar('.');
        i = 0;
    }
    IFS0bits.T3IF = 0;
}