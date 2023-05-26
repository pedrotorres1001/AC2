#include <detpic32.h>

void putc(char byte){
    while(U2STAbits.UTXBF == 1);
    U2TXREG = byte;
}

void delay(unsigned int ms){
    resetCoreTimer();
    while(readCoreTimer() < 20000 * ms);
}

int main(void){
    U2BRG = ((PBCLK + (8 * 115200))/(16 * 115200)) - 1;

    U2MODEbits.BRGH = 0; // 16x baud clock enabled
    U2MODEbits.PDSEL = 0; // 8 bits sem paridade
    U2MODEbits.STSEL = 0; // 1 stop bit

    U2STAbits.UTXEN = 1; // enable ao transmiter
    U2STAbits.URXEN = 1; // enable ao receiver

    U2MODEbits.ON = 1; // enable UART2

    while(1){
        putc('+');
        delay(1000);
    }

    return 0; 
}