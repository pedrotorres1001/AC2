#include <detpic32.h>
#define msVal 20000

volatile unsigned char voltage = 0; // Global variable

void delay (unsigned int ms);
void send2displays(unsigned char value);
unsigned char toBcd(unsigned char value);


int main(void) {
    
    unsigned int cnt = 0;
    //Configure outputs and stuff
    //enable the 7seg display
    TRISB = 0x80FF;
    TRISD = 0xFF9F;

    TRISBbits.TRISB4 = 1;    // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0;   // RB4 configured as analog input (AN4)
    AD1CON1bits.SSRC = 7;    
    
    AD1CON2bits.SMPI = 7;    // 8 sample will be converted and stored in buffer locations ADC1BUF0  
    AD1CHSbits.CH0SA = 4;    // Selects AN4 as input for the A/D converter
    AD1CON1bits.ON = 1;      // Enable A/D converter
    

    // Configure interrupt system
    IPC6bits.AD1IP = 2; // configure priority of A/D interrupts
    IFS1bits.AD1IF = 0; // clear A/D interrupt flag
    IEC1bits.AD1IE = 1; // enable A/D interrupts
    EnableInterrupts(); // Global Interrupt Enable

    while(1) {
        if (cnt == 0) {
            //Start A/D conversion
            AD1CON1bits.ASAM = 1; // Start conversion   
        }
        // Send "voltage" variable (decimal) to displays
        send2displays(toBcd(voltage));
        cnt = (cnt + 1) % 25;
        delay(10); 
    }
    return 0;
}

// Interrupt Handler
void _int_(27) isr_adc(void) {
    
    // Read 8 samples (ADC1BUF0, ..., ADC1BUF7) and calculate the average
    int *p = (int *)(&ADC1BUF0);
    int i;
    int sum = 0;
    for (i = 0; i < 8; i++) {
        sum += p[i*4];
    }
    int average = sum / 8;

    // Calculate voltage amplitude
    voltage = (average * 33 + 511) / 1023;

    // Start A/D conversion
    AD1CON1bits.ASAM = 1; // Start conversion
    IFS1bits.AD1IF = 0; // Reset AD1IF flag
}

void send2displays(unsigned char value) {

    static int displayFlag = 0;
    static const char display7Scodes[] = {0x3F, 0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

    if (displayFlag == 0) {
        // select display high
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;
        // send digit_high (dh) to display: dh = value >> 4
        LATB = (LATB & 0x80FF) | (display7Scodes[value >> 4] << 8);
        displayFlag = 1;
    } else {
        // select display low
        LATDbits.LATD5 = 1;
        LATDbits.LATD6 = 0;
        // send digit_low (dl) to display: dl = value & 0x0F
        LATB = (LATB & 0x80FF) | (display7Scodes[value & 0x0F] << 8);
        displayFlag = 0;
    }
}

void delay (unsigned int ms) {
    unsigned int wait;
    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}
