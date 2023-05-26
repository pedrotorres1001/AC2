#include <detpic32.h>

void init() {
    T3CONbits.TCKPS = 7;    // 1:32 prescaler (i.e. fout_presc = 625 KHz)
    PR3 = 39061;            // Fout = 20MHz / (32 * (62499 + 1)) = 10 Hz
    TMR3 = 0;               // Clear timer T2 count register
    T3CONbits.TON = 1;      // Enable timer T2 (must be the last command of the
                            // timer configuration sequence
}

int main(void){
    init();
    while(1){
        while (IFS0bits.T3IF == 0);
        IFS0bits.T3IF = 0;
        putChar('.');
    }
    return 0;
}
