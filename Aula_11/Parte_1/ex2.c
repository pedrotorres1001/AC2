#include <detpic32.h>

void putc(char byte){
    // wait while UTXBF == 1
    while(U3STAbits.UTXBF == 1);
    // Copy byte to the UxTXREG register
    U2TXREG = byte;
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
    
    while(1);
    return 0;
}

void _int_(32) isr_uart2(void)
{
    if (IFS1bits.U2RXIF == 1)
    {
        // Read character from FIFO (U2RXREG)
        // Send the character using putc()
        // Clear UART2 Rx interrupt flag
        putc(U2RXREG);
        IFS1bits.U2RXIF = 0;
    }
}