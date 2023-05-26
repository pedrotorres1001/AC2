#include <detpic32.h>

typedef struct
{
    char mem[100]; // Storage area
    int nchar; // Number of characters to be transmitted
    int posrd; // Position of the next character to be transmitted
} t_buf;

volatile t_buf txbuf;

void putc(char byte){
    // wait while UTXBF == 1
    while(U2STAbits.UTXBF == 1);
    // Copy byte to the UxTXREG register
    U2TXREG = byte;
}

// ex1
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
    // Configure UART2: 115200, N, 8, 1
    // Configure UART2 interrupts, with RX interrupts enabled
    // and TX interrupts disabled:
    // enable U2RXIE, disable U2TXIE (register IEC1)
    // set UART2 priority level (register IPC8)
    // clear Interrupt Flag bit U2RXIF (register IFS1)
    // define RX interrupt mode (URXISEL bits)
    // Enable global Interrupts

    // Configure UART2:
    // 1 - Configure BaudRate Generator (115200 given baudrate)
    U2BRG = ((PBCLK + 8 * 115200) / (16 * 115200)) - 1;
    
    // Configure number of data bits, parity and number of stop bits
    // (see U2MODE register)
    U2MODEbits.BRGH = 0; // 16x baud clock enabled
    U2MODEbits.PDSEL = 0; // 8 bits, no parity
    U2MODEbits.STSEL = 0; // 1 stop bit

    // Enable the trasmitter and receiver modules
    U2STAbits.UTXEN = 1; // Enable transmitter (enviar)
    U2STAbits.URXEN = 1; // Enable receiver (receber)

    // Enable UART2
    U2MODEbits.ON = 1; // Enable UART2

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

    // ex3
    while(1){
        {
            putstrInt("Test string which can be as long as you like, up to a maximum of 100 characters\n");
        }
    }
    return 0;
}

// ex2
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
