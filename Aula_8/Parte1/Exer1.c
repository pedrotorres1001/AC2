#include <detpic32.h>

int main(void) {
    T3CONbits.TCKPS = 7; //256 = 2^8
    PR3 = 62499; // Fout = 20MHz / (32 * (62499 + 1)) = 2 Hz
    TMR3 = 0; // Clear timer T3 count register
    T3CONbits.TON = 1; // Enable timer T3 (must be the last command of the
    // timer configuration sequence)

    while(1)
    {
        // Wait while T3IF = 0
        while(IFS0bits.T3IF == 0);
        // Reset T3IF
        IFS0bits.T3IF == 0;
        putChar('.');
    }
    return 0;
}
