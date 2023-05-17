/* #include <detpic32.h>
#define nConvertions 4
#define msVal 20000

volatile int voltage = 0;

void delay (unsigned int ms) {
    unsigned int wait;
    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}

int main(void)
{
    // Configure all (digital I/O, analog input, A/D module)
    TRISBbits.TRISB4 = 1;    // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0;   // RB4 configured as analog input (AN4)
    AD1CON1bits.SSRC = 7;
    AD1CON2bits.SMPI = 3;    // 4 samples will be converted and stored in buffer locations ADC1BUF0  
    AD1CHSbits.CH0SA = 4;    // Selects AN4 as input for the A/D converter
    AD1CON1bits.ON = 1;      // Enable A/D converter
    i = 0;
    while(1)
    {
        if(i == 0) // 0, 200ms, 400ms, 600ms, ...
        {
            // Convert analog input (4 samples)
            // Read samples and calculate the average
            // Calculate voltage amplitude
            // Convert voltage amplitude to decimal
            AD1CON1bits.ASAM = 1; // Start conversion

            while (IFS1bits.AD1IF == 0); // Wait while conversion not done (AD1IF == 0)

            for (i = 0; i < 16; i++) {
                media = (ADC1BUF0 + ADC1BUF1 + ADC1BUF2 + ADC1BUF3) / 4;
                /* voltage = (((33 - 0) * (media)) / 1023 ) + 0;
                printInt(voltage, 10 | 3 << 16);
                putChar(' ');
            }
            // Send voltage value to displays
            // Wait 10 ms (using the core timer)
            i = (i + 1) % 25;
        }
    }
    return 0;
}
*/

#include <detpic32.h>
#define msVal 20000

void delay(unsigned int);

void send2displays(unsigned char value) {
    static const char display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    static char displayFlag = 0;

    int digit_low = value & 0x0F;
    int digit_high = value >> 4;

    if (displayFlag == 0) {
        LATDbits.LATD5 = 1; 
        LATDbits.LATD6 = 0;
        LATB = (LATB & 0x00FF) | (display7Scodes[digit_low] << 8);
        displayFlag = 1;
    } else {
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;
        LATB = (LATB & 0x00FF) | (display7Scodes[digit_high] << 8);
        displayFlag = 0;
    }
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

int main(void) {
    int media, V, *p, Vdecimal;
    TRISBbits.TRISB4 = 1; // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0; // RB4 configured as analog input (AN
    AD1CON1bits.SSRC = 7; // Conversion trigger selection bits: in this
    // mode an internal counter ends sampling and
    // starts conversion
    AD1CON2bits.SMPI = 5; // Interrupt is generated after N samples
    // (replace N by the desired number of
    // consecutive samples)
    AD1CHSbits.CH0SA = 4; // replace x by the desired input
    // analog channel (0 to 15)
    AD1CON1bits.ON = 1; // Enable A/D converter
    // This must the last command of the A/D
    // configuration sequence
    TRISB = 0x80FF | 0x0001;
    TRISD = 0xFF9F;
    int i = 0;
    while(1) {
        AD1CON1bits.ASAM = 1; // Start conversion
        p = (int *)(&ADC1BUF0);
        if (i == 0) {
            for(; p <= (int *)(&ADC1BUFF); p+=4 ) {
                media = (ADC1BUF0 + ADC1BUF1 + ADC1BUF2 + ADC1BUF3) / 4;
                V = (((33 - 0) * (media )) / 1023 ) + 0;
                Vdecimal = (V / 10) * 16 + (V % 10);
            }
        }
        send2displays(Vdecimal);
        delay(10);
        i = (i + 1) % 25;
    }
    return 0;
}

void delay (unsigned int ms) {
    unsigned int wait;

    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}