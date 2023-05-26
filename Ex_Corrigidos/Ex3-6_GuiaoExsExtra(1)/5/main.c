#include <detpic32.h>
#define BAUDRATE 1200

void delay(unsigned int ms) {
        resetCoreTimer();
        while(readCoreTimer() < 20000*ms);
}

void init() {
        // Configure UART2 (1200, N, 8, 1):
        U2BRG = (PBCLK + (8 * BAUDRATE)) / (16 * BAUDRATE) - 1;
        U2MODEbits.BRGH = 0;
        U2MODEbits.PDSEL = 0;   // 8 bits no parity
        U2MODEbits.STSEL = 0;   // 1 bit stop
        U2STAbits.UTXEN = 1;    // transmiter ativo
        U2STAbits.URXEN = 0;    // receiver ativo
        U2MODEbits.ON = 1;      // UART2 ativo

        TRISB = 0x000F;
}

void putc(char byte1) {
        while(U2STAbits.UTXBF == 1);    // wait while UTXBF == 1
        U2TXREG = byte1;                // Copy byte2send to the UxTXREG register
}

void putstring(char *str) {
        int i;
        for(i = 0; str[i] != '\0'; i++) {       // neste ciclo estamos a percorrer a string até ao seu terminador
                putc(str[i]);                   // é adicionado carater a carater o que estiver na string
        }
}

int main(void) {
        init();
        while(1) {
                putstring("RB30 = ");
                putc(PORTBbits.RB3 + '0');      // adiciona o código ascii de 0 (0x42) ao valor que pode vir de RBx e ou dá putc a 0x42 (0) ou 0x43 (1)
                putc(PORTBbits.RB2 + '0');
                putc(PORTBbits.RB1 + '0');
                putc(PORTBbits.RB0 + '0');
                putc('\n');
                delay(500);                     // 500 ms = 2Hz
        }
        return 0;
}
