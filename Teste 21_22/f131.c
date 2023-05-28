#include <detpic32.h>

// 9600,O,8,2
// contador de módulo 16
// resultado nos leds RE4 a RE1

volatile int counter = 0;

void send2Leds(int counter){

    LATE = (LATE & 0xFFE1) | counter;

}

void putc(char byte) {
    while (U2STAbits.UTXBF == 1);
    U2TXREG = byte;
}

char getc(void) {
    while(U2STAbits.URXDA == 0);
    return U2RXREG;
}

void putstr(char *str) {
    int i;
    for (i = 0; str[i] != '\0'; i++){
        putc(str[i]);
    }
}

void init() {

    TRISE = TRISE & 0xFFE1;

    U2BRG = ((PBCLK + 8 * 9600) / (16 * 9600)) - 1;

    U2MODEbits.BRGH = 0;
    U2MODEbits.PDSEL = 0b10; // 8 bits, odd parity
    U2MODEbits.STSEL = 1; // 2 stop bits

    U2STAbits.URXEN = 1;
    U2STAbits.UTXEN = 1;
    U2MODEbits.ON = 1;

    // CONFIGURAR A INTERRUPÇÃO
    // Enable UART2 RX and TX interrupt
    IEC1bits.U2RXIE = 1; // Receive Interrupt Enable bit
    IEC1bits.U2TXIE = 0; // Transmit Interrupt Enable bit
    // Configure priority of interrupts
    IPC8bits.U2IP = 1; // Priority bits
    // Clear UART2 rx interrupt flag
    IFS1bits.U2RXIF = 0; // Receive Interrupt Flag bit
    // define RX interrupt mode (URXISEL bits)
    U2STAbits.URXISEL = 0; // Interrupt is set when any character is received
    // Enable interrupts
    EnableInterrupts();

}

int main(void) {
    init();
    while(1) {
        send2Leds(counter);
    }
    return 0;
}

void _int_(32) isr_uart2(void)
{
    char c;
    c = getc();
    if (c == 'U'){
        counter++;
    } else if ((c == 'R') || (counter == 15)) {
        putstr("RESET");
        putc('\n');
        counter = 0;
    }
    
    IFS1bits.U2RXIF = 0;
}
