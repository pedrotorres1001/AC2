#include <detpic32.h>

int main(void) {

    U2BRG = ((PBCLK + (8 * 115200))/(16 * 115200)) - 1;

    U2MODEbits.BRGH = 0;
    U2MODEbits.PDSEL = 0;
    U2MODEbits.STSEL = 0;

    U2STAbits.UTXREG = 1;
    U2STAbits.URXREG = 1;

    U2MODEbits.ON = 1;

    ICE1bits.U2RXIE = 1;
    IEC1bits.U2TXIE = 0;

    IPC8bits.U2IP = 1;

    IFS1bits.U2RXIF = 0;

    U2STAbits.URXISEL = 0;

    EnableInterrupts();

    while(1);
    return 0;
    
}