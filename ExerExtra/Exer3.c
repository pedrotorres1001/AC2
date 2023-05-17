#include <detpic32.h>

int main(void) {

    TRISB = 0x000F; // configure RB0-RB3 as inputs
    TRISE = 0xFFF0; // configure RE0-RE3 as outputs

    while (1) {
        LATE = (LATE & 0xFFF0) | (PORTB & 0x000F);
    }
    return 0;
}