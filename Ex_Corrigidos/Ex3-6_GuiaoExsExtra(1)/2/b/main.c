#include <detpic32.h>

volatile int value = 0;

void delay(unsigned int ms) {

    resetCoreTimer();
    while(readCoreTimer() < 20000 * ms);
}

int toBCD (unsigned int val) {

    return (((value / 10) << 4 ) + (value % 10));
}

void send2displays(unsigned char value)
{
	static const char display7Scodes[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };

	static char flag = 0;

	if (flag == 1) {
		// select display high
		// send digit_high (dh) to display:	dh = value >> 4;
		LATD = (LATD & 0xFF9F) | 0xFFDF;
		LATB = (LATB & 0x80FF) | (display7Scodes[(value >> 4)] << 8);
		flag = 0;
	} else {
		// select display low
		// send digit_low (dl) to display:	dl = value & 0x0F;
		LATD = (LATD & 0xFF9F) | 0xFFBF;
		LATB = (LATB & 0x80FF) | (display7Scodes[(value & 0x0F)] << 8);
		flag = 1;
	}
}

int main () {

    int c, freq;
    int freqVal[5] = {39061, 19530, 13019, 9764, 7811};

    T1CONbits.TCKPS = 3; // 1:32 prescaler (i.e. fout_presc = 625 KHz)
    PR1 = 62499; // Fout = 20MHz / (32 * (62499 + 1)) = 10 Hz
    TMR1 = 0; // Clear timer T2 count register
    IPC1bits.T1IP = 2; // Interrupt priority (must be in range [1..6])
    IEC0bits.T1IE = 1; // Enable timer T2 interrupts
    T1CONbits.TON = 1; // Enable timer T2 (must be the last command of the timer configuration sequence)
    
    T2CONbits.TCKPS = 3; // 1:32 prescaler (i.e. fout_presc = 625 KHz)
    PR2 = 49999; // Fout = 20MHz / (32 * (62499 + 1)) = 10 Hz
    TMR2 = 0; // Clear timer T2 count register
    IPC2bits.T2IP = 2; // Interrupt priority (must be in range [1..6])
    IEC0bits.T2IE = 1; // Enable timer T2 interrupts
    T2CONbits.TON = 1; // Enable timer T2 (must be the last command of the timer configuration sequence)

    EnableInterrupts();

    TRISB = 0x80FF;
    TRISD = 0x9F;

    while (1) {

        c = inkey();

        if ((c > 0) || (c < 4)) {

            c = c - '0';
            freq = (2 * (1 + c));
            PR1 = freqVal[c];
            printStr("Nova frequência: ");
            putChar(freq + '0');
            putChar('\n');
        }
        printInt(value, 16 | 2 << 16);
        putChar('\r');
    }

    return 0;
}

void _int_(4) isr_t1 (void) {

    if (value == 100) {
        value = 0;
    } else {
        value++;
    }

    IFS0bits.T1IF = 0;
}

void _int_(8) isr_t2 (void) {

    send2displays(toBCD(value));
    IFS0bits.T2IF = 0;
}