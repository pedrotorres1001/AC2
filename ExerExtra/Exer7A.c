#include <detpic32.h>
#define msVal 20000

void delay(int ms);

int main(void) {

    TRISB = 0x000F;

    while (1) {
        int c = PORTB & 0x000F;
        int i;
        for (i = 3; i >= 0; i--) {
            int bit = (c >> i) & 0x0001;
            if(i == 3) {
                printStr("DS4=(");
            }
            else if (i == 2) {
                printStr("DS3=(");
            }
            else if (i == 1) {
                printStr("DS2=(");
            }
            else {
                printStr("DS1=(");
            }
            putChar(bit + '0');
            printStr("), ");
        }
        putChar('\n');
        putChar('\r');
        delay(1000);
    }
}

void delay(int ms) {
    unsigned int wait;

    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}