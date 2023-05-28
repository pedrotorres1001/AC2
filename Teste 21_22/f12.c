#include <detpic32.h>

volatile unsigned char voltage = 0;

// 10 hz = 100 ms
// 4 samples

void delay(unsigned int ms) {
    resetCoreTimer();
    while (readCoreTimer() < 20000 * ms);
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

void init() {
    TRISBbits.TRISB4 = 1; // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0; // RB4 configured as analog input
    AD1CON1bits.SSRC = 7; // Conversion trigger selection bits: in this
    // mode an internal counter ends sampling and
    // starts conversion
    AD1CON1bits.CLRASAM = 1; // Stop conversions when the 1st A/D converter
    // interrupt is generated. At the same time,
    // hardware clears the ASAM bit
    AD1CON2bits.SMPI = 3; // Interrupt is generated after N samples
    // (replace N by the desired number of
    // consecutive samples)
    AD1CHSbits.CH0SA = 4; // replace x by the desired input
    // analog channel (0 to 15)
    AD1CON1bits.ON = 1; // Enable A/D converter
    // This must the last command of the A/D
    // configuration sequence

    // Timer 2
    T2CONbits.TCKPS = 2;            // 1:4
    PR2 = 49999;                    // (20 MHz / 4[Prescaler 1:4] * 100[Hz]) - 1 = 49999
    TMR2 = 0;
    T2CONbits.ON = 1;

    // T2 PRIO
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;
    IFS0bits.T2IF = 0;

    IPC6bits.AD1IP = 2; // configure priority of A/D interrupts
    IFS1bits.AD1IF = 0; // clear A/D interrupt flag
    IEC1bits.AD1IE = 1; // enable A/D interrupts

    TRISE = TRISE & 0xFF00;
    TRISB = TRISB & 0x80FF;
    TRISD = TRISD & 0xFF9F;
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

int main(void){
    init();
    EnableInterrupts();
    while(1) {
        AD1CON1bits.ASAM = 1; // Start conversion   
        delay(100); 
    }
    return 0;
}

void _int_(27) isr_adc(void) {
    
    int soma = 0;
    int i;
    int *p = (int *) (&ADC1BUF0);
    for (i = 0; i < 4; i++){
        soma += p[i*4];
    }
    int average = soma / 4;

    voltage = (average * 16 + 511) / 1023;

    if ((voltage >= 1) & (voltage <= 8)) {
        if (voltage == 1){
            LATE = (LATE & 0xFF00) | 0x0001;
        }
        else if (voltage == 2){
            LATE = (LATE & 0xFF00) | 0x0003;
        }
        else if (voltage == 3) {
            LATE = (LATE & 0xFF00) | 0x0007;
        }
        else if (voltage == 4) {
            LATE = (LATE & 0xFF00) | 0x000F;
        }
        else if (voltage == 5) {
            LATE = (LATE & 0xFF00) | 0x001F;
        }
        else if (voltage == 6) {
            LATE = (LATE & 0xFF00) | 0x003F;
        }
        else if (voltage == 7) {
            LATE = (LATE & 0xFF00) | 0x007F;
        }
        else {
            LATE = (LATE & 0xFF00) | 0x00FF;
        }
    }
    else {
        LATE = (LATE & 0xFF00) | 0x0000;
    }   
    AD1CON1bits.ASAM = 1; // Start conversion
    IFS1bits.AD1IF = 0; // Reset AD1IF flag
}

void _int_(8) isr_T2(void)
{
    send2displays(toBcd(voltage));
    IFS0bits.T2IF = 0;
}

