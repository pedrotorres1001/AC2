#include <detpic32.h>

// 9600,E,8,1 
// mostrar todos os caracteres introduzidos
// 0x0A (enter) para parar e apresentar o número de caracteres maiusculos
// putc e putstr 
// interrupção da UART2 
// o processamento da receção de caracteres deve ser feito na rotina de serviço de interrupção

volatile int count = 0;
char c;

void putc(char byte) {
    while (U2STAbits.UTXBF == 1);
    U2TXREG = byte;
}

void putstr(char *str) {
    int i;
    for (i = 0; str[i] != '\0'; i++){
        putc(str[i]);
    }
}

char getc(void) {
    while(U2STAbits.URXDA == 0);
    return U2RXREG;
}

void init() {
    
    U2BRG = ((PBCLK + 8 * 9600) / (16 * 9600)) - 1;

    U2MODEbits.BRGH = 0;
    U2MODEbits.PDSEL = 0b10;
    U2MODEbits.STSEL = 0;

    U2STAbits.URXEN = 1;
    U2STAbits.UTXEN = 1;

    U2MODEbits.ON = 1;

    // Configurar a interrupção

    IEC1bits.U2RXIE = 1; // Receive Interrupt Enable bit
    IEC1bits.U2TXIE = 0; // Transmit Interrupt Enable bit
    // Configure priority of interrupts
    IPC8bits.U2IP = 1; // Priority bits
    // Clear UART2 rx interrupt flag
    IFS1bits.U2RXIF = 0; // Receive Interrupt Flag bit
    // define RX interrupt mode (URXISEL bits)
    U2STAbits.URXISEL = 0; // Interrupt is set when any character is received
    // Enable interrupts

}

int main(void) {
    init();
    EnableInterrupts();
    while(1);
    return 0;
}

void _int_(32) isr_uart2(void)
{
    if (IFS1bits.U2RXIF == 1)
    {
        c = getc();
        if (c == 0x0A) {
            putc('\n');
            putc('\r');
            putstr("O número de minúsculas digitado foi: ");
            if(count >= 10) {
                putc(count/10 + '0');
                putc(count%10 + '0');
            } else {
                putc(count + '0');
            }
            putc('.');
            putc('\n');
            putc('\r');
            count = 0;
        } else {
            putc(c);
            if (c >= 'a' && c <= 'z') {
                count++;
            }
        }
        IFS1bits.U2RXIF = 0;
    }
}

