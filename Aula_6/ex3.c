#include <detpic32.h>

int main(void) {

    int i;
    TRISBbits.TRISB4 = 1;    // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0;   // RB4 configured as analog input (AN4)
    AD1CON1bits.SSRC = 7;
    AD1CON2bits.SMPI = 0;    // 1 sample will be converted and stored in buffer locations ADC1BUF0  
    AD1CHSbits.CH0SA = 4;    // Selects AN4 as input for the A/D converter
    AD1CON1bits.ON = 1;      // Enable A/D converter

    while (1) {

        AD1CON1bits.ASAM = 1; // Start conversion

        while (IFS1bits.AD1IF == 0); // Wait while conversion not done (AD1IF == 0)

	int *p = (int *) (&ADC1BUF0);
	for (i = 0; i < 16; i++) {
		printInt(p[i*4], 16 | 3 << 16);
		putChar(' ');
	}
        IFS1bits.AD1IF = 0; // Reset AD1IF flag
    }

    return 0;
}
