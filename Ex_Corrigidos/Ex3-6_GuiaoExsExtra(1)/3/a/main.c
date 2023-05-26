#include <detpic32.h>

// Escreva um programa para gerar na saída OC3 um sinal PWM, com uma frequência de 1000 Hz,
// cujo duty-cycle seja controlado pelo potenciómetro que está ligado à entrada AN4 da ADC do
// PIC32: ao valor máximo da conversão deve corresponder o duty-cycle 100% e ao valor mínimo
// deve corresponder o duty-cycle 0%. A frequência de amostragem da ADC deve ser 10 Hz, obtida
// através da utilização do Core Timer. O fim de conversão da ADC deve ser processado por polling.
// Utilize o Timer 3 como referência para a geração do sinal PWM.

void delay(unsigned int ms) {
        resetCoreTimer();
        while(readCoreTimer() < 20000 * ms);
}

void init() {
        T3CONbits.TCKPS = 0;    // 1:1 prescaler (i.e. fout_presc = 20 MHz)
        PR3 = 19999;            // PR3 = (20 000 000 / 1) / 1000 = 19999;  
        TMR3 = 0;               // Clear timer T3 count register
        T3CONbits.TON = 1;      // Enable timer T2 (must be the last command of the
                                // timer configuration sequence)
        IPC3bits.T3IP = 2;      // Interrupt priority (must be in range [1..6])
        IEC0bits.T3IE = 1;      // Enable timer T2 interrupts
        IFS0bits.T3IF = 0;      // Reset timer T2 interrupt flag

        OC3CONbits.OCM = 6;     // PWM mode on OCx; fault pin disabled
        OC3CONbits.OCTSEL = 1;  // Use timer T2 as the time base for PWM generation
        OC3RS = 20000;          // Ton constant OC3RS = ((PR3 + 1) * 100) / 100
        OC3CONbits.ON = 1;      // Enable OC3 module

        TRISBbits.TRISB4 = 1;           // RB4 digital output disconnected
        AD1PCFGbits.PCFG4 = 0;          // RB4 configured as analog input
        AD1CON1bits.SSRC = 0;           // Conversion trigger selection bits: in this
                                        // mode an internal counter ends sampling and
                                        // starts conversion
        AD1CON1bits.CLRASAM = 1;        // Stop conversions when the 1st A/D converter
                                        // interrupt is generated. At the same time,
                                        // hardware clears the ASAM bit
        AD1CON3bits.SAMC = 16;          // Sample time is 16 TAD (TAD = 100 ns)
        AD1CON2bits.SMPI = 0;           // Interrupt is generated after N samples
                                        // (replace N by the desired number of
                                        // consecutive samples)
        AD1CHSbits.CH0SA = 4;           // replace x by the desired input
                                        // analog channel (0 to 15)
        AD1CON1bits.ON = 1;             // Enable A/D converter
                                        // This must the last command of the A/D
                                        // configuration sequence
}

void setPWM(unsigned int dutycycle) {
        if(dutycycle >= 0 && dutycycle <= 100) {
                OC3RS = 200 * dutycycle;  // Ton constant  OC3RS = ((PR3 + 1) * duty_cycle) / 100
        }
}

int main(void) {
        init();
        unsigned int value = 0;
        
        while(1) {
                AD1CON1bits.ASAM = 1;   // iniciar a conversão
                while(IFS1bits.AD1IF == 0);     // enquanto a flag estiver a 0
                value = (100 * ADC1BUF0 + 511) / 1023;  // cálculo do valor
                setPWM(value);                  // o nosso duty_cycle passa a tomar o valor calculado anteriormente
                IFS1bits.AD1IF = 0;             // reset da flag
                delay(100);                     // esperar 100 ms (10Hz)
        }


}