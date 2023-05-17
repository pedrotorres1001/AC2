#include <detpic32.h>
#define msVal 20000

void delay(unsigned int ms) {
    resetCoreTimer();
    while (readCoreTimer() < msVal * ms);
}

void putc(char byte){
    // wait while UTXBF == 1
    while(U2STAbits.UTXBF == 1);
    // Copy byte to the UxTXREG register
    U2TXREG = byte;
}

char getc(void){
    while(U2STAbits.URXDA == 0); // Wait while URXDA == 0
    return U2RXREG; // Return U2RXREG
}

void puts(char *str){
    while (*str != '\0'){
        putc(*str);
        str++;
    }
}

int main(void){
    // Configure UART2:
    // 1 - Configure BaudRate Generator
    // 2 – Configure number of data bits, parity and number of stop bits
    // (see U2MODE register)
    // 3 – Enable the trasmitter and receiver modules (see register U2STA)
    // 4 – Enable UART2 (see register U2MODE)
    // consider a factor of division of 16

    // Configure UART2:
    // 1 - Configure BaudRate Generator (115200 given baudrate)
    U2BRG = ((PBCLK + 8 * 115200) / (16 * 115200)) - 1;
    
    // Configure number of data bits, parity and number of stop bits
    // (see U2MODE register)
    U2MODEbits.BRGH = 0; // 16x baud clock enabled
    U2MODEbits.PDSEL = 0; // 8 bits, no parity
    U2MODEbits.STSEL = 0; // 2 stop bit

    // Enable the trasmitter and receiver modules
    U2STAbits.UTXEN = 1; // Enable transmitter (enviar)
    U2STAbits.URXEN = 1; // Enable receiver (receber)

    // Enable UART2
    U2MODEbits.ON = 1; // Enable UART2

    while(1){
        // Wait while TRMT == 0
        while(U2STAbits.TRMT == 0);
        // Set RD11
        LATDbits.LATD11 = 1;
        puts("12345");
        // Reset RD11
        LATDbits.LATD11 = 0;
    }
    return 0;

}