#include <detpic32.h>
#define msVal 20000

void delay(unsigned int);

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

int main(void) {
    int i = 0;
    while(1) {
        if (i >= 100) {
            i = 0;
        }
        printInt(toBcd(i), 16 | 2 << 16);
        putChar('\r');
        delay(100);
        i++;
    }
    return 0;
}

void delay (unsigned int ms) {
    unsigned int wait;

    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}