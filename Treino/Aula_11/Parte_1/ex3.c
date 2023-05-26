#include <detpic32.h>

void putc(char byte){
    while(U2STAbits.UTXBF == 1);
    U2TXREG = byte;
}

int main(void) {

    U2BRG = ((PBCLK + (8 * 115200))/(16 * 115200)) - 1;

    U2MODEbits.BRGH = 0;
    U2MODEbits.PDSEL = 0;
    U2MODEbits.STSEL = 0;

    U2STAbits.UTXEN = 1;
    U2STAbits.URXEN = 1;

    U2MODEbits.ON = 1;

    IEC1bits.U2RXIE = 1;
    IEC1bits.U2TXIE = 0;

    IPC8bits.U2IP = 1;

    IFS1bits.U2RXIF = 0;

    U2STAbits.URXISEL = 0;

    EnableInterrupts();

    while(1);
    return 0;
    
}

void _int_(32) isr_uart2(void)
{
    char cc;
    if (IFS1bits.U2RXIF == 1)
    {
        cc = U2RXREG;
        putc(cc);

        if (cc == 'a'){
            LATCbits.LATC14 = 1;
        }
        else if (cc == 'c'){
            LATCbits.LATC14 = 0;
        }
        IFS1bits.U2RXIF = 0;
    }
}