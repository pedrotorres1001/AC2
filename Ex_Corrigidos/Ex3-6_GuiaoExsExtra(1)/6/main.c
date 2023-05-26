#include <detpic32.h>
#define BAUDRATE 57600

// Escreva um programa que leia, à frequência de 1.5 Hz, o valor dos 4 bits dos portos RB3 a RB0 e
// que guarde esse valor num buffer circular com 16 posições (quando o buffer ficar cheio deve ser
// descartado o valor mais antigo). Para gerar esta temporização deve usar o Core Timer.
// Configure a UART2 com os parâmetros de comunicação 57600 bps, 8 data bits, paridade ímpar, 1
// stop bit, e escreva as funções de transmissão de um carater e de transmissão de uma string, por
// polling. Acrescente ainda o código necessário para processar a receção de um caracter por
// interrupção.
// Quando for recebido o carater ‘D’ deve ser enviado para o PC o número de valores armazenados
// no buffer circular, seguido desses valores, enviados por ordem cronológica de ocorrência.
// Adicionalmente, o número de valores do buffer deve ser reposto a zero.
// Sugestão: transforme o valor armazenado numa dada posição do buffer circular numa string
// binária e use a função de envio de uma string para fazer a transmissão do valor para o PC (por
// exemplo: #elem: 5 – 1001, 1001, 0000, 0011, 1010).

typedef struct
{
    char mem[100]; // Storage area
    int nchar; // Number of characters to be transmitted
    int posrd; // Position of the next character to be transmitted
} t_buf;

volatile t_buf txbuf;

void delay(unsigned int ms) {
        resetCoreTimer();
        while(readCoreTimer() < 20000 * ms);
}

void init() {
        // Configure UART2 (1200, N, 8, 1):
        U2BRG = (PBCLK + (8 * BAUDRATE)) / (16 * BAUDRATE) - 1;
        U2MODEbits.BRGH = 0;
        U2MODEbits.PDSEL = 1;   // Impar
        U2MODEbits.STSEL = 0;
        U2STAbits.UTXEN = 1;
        U2STAbits.URXEN = 0;
        U2MODEbits.ON = 1;

        TRISB = TRISB | 0x000F;
        // Configure UART2 interrupts
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

void putc(char byte1) {
        while(U2STAbits.UTXBF == 1);    // wait while UTXBF == 1
        U2TXREG = byte1;                 // Copy byte2send to the UxTXREG register
}

char getc(void)
{
        while(U2STAbits.URXDA == 0);    // Wait while URXDA == 0
        return U2RXREG;
}

void putstrInt(char *s){
    while(txbuf.nchar > 0); // Wait while the buffer is not empty
    // Copy all characters of the string "s" to the buffer
    while(*s != '\0') {
        txbuf.mem[txbuf.nchar] = *s;
        txbuf.nchar++;
        s++;
    }
    // Initialize "posrd" variable with 0
    txbuf.posrd = 0;
    // Enable UART2 Tx interrupts
    IEC1bits.U2TXIE = 1;
}

int main(void) {
        init();
        while(1) {
                delay(667);              // 1.5Hz
        }
        return 0;
}

void _int_(32) isr_uart2(void)
{
    if (IFS1bits.U2TXIF == 1)
    {
        if(txbuf.nchar > 0) { // At least one character to be transmitted
            // U2TXREG = ... // Read 1 character from the buffer and
            // send it
            // Update buffer "posrd" and "nchar" variables
            U2TXREG = txbuf.mem[txbuf.posrd];
            txbuf.posrd++;
            txbuf.nchar--;
        } else {
            // Disable UART2 Tx interrupts
            IEC1bits.U2TXIE = 0;
        }
    // Clear UART2 Tx interrupt flag
    IFS1bits.U2TXIF = 0;
    }
}