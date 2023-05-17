#include <detpic32.h>

int main(void)
{
    
    TRISB = 0x80FF; // 1000 0000 1111 1111
    TRISD = 0xFF9F; // 1111 1111 1001 1111
    LATB = LATB & 0x80FF;
    char c;
    // TRISBbits.TRISB8 = 0;
    // TRISBbits.TRISB14 = 0;
    // TRISDbits.TRISD6 = 0;
    // TRISDbits.TRISD5 = 1;
    LATDbits.LATD5 = 0;
    LATDbits.LATD6 = 1;
    while(1)
    {
        c = getChar();
        if (c >= 'a' && c <= 'g' ) {
            LATB = (LATB & 0x80FF) | (1 << (8 + (c - 'a'))) ; // 1000 0000 1111 1111
            putChar(c);
        } else if (c >= 'A' && c <= 'G') {
            LATB = (LATB & 0x80FF) | (1 << (8 + (c - 'A'))) ; // 1000 0000 1111 1111
            putChar(c);
        }
    }
}
