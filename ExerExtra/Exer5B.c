#include <detpic32.h>

#define msVal 20000

void delay(unsigned int);

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

int main(void) {
    int i = 0;
    int c;
    int freq = 0;

    while(1) {
        c = getChar();
        
        if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4') {
            freq = 2 * ( 1 + c );
            if (i >= 100) {
                i = 0;
            }
            putChar('\r');
            printInt(toBcd(i), 16 | 2 << 16);
            putChar(' ');
            printInt(freq, 16 | 2 << 16);
            delay((1/(toBcd(freq))) * 1000);
            i++;
        } else {
            if (i >= 100) {
                i = 0;
            }
            putChar('\r');
            printInt(toBcd(i), 16 | 2 << 16);
            putChar(' ');
            i++;
        }
         
    }
    return 0;
}

void delay (unsigned int ms) {
    unsigned int wait;

    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}