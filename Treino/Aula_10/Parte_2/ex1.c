#include <detpic32.h>

void putc(char byte){
    while(U2STAbits.UTXBF == 1);
    U2TXREG = byte;
}

void delay(unsigned int ms){
    resetCoreTimer();
    while(readCoreTimer() < 20000 * ms);
}

void putstr(char *str){
    while(*str != '\0'){
        putc(*str);
        str++;
    }
}

char getc(void){
    while (U2STAbits.URXDA == 0);
    return U2RXREG;
}

void putc1(char byte){
    while(U1STAbits.UTXBF == 1);
    U1TXREG = byte;
}

int main(void){
    U1BRG = ((PBCLK + (8 * 115200))/(16 * 115200)) - 1;

    U1MODEbits.BRGH = 0; // 16x baud clock enabled
    U1MODEbits.PDSEL = 0; // 8 bits sem paridade
    U1MODEbits.STSEL = 0; // 1 stop bit

    U1STAbits.UTXEN = 1; // enable ao transmiter
    U1STAbits.URXEN = 1; // enable ao receiver

    U1MODEbits.ON = 1; // enable UART1

    while(1){
        putc1(0x5A);
        delay(10);
    }

    return 0; 
}