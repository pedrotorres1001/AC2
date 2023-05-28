#include <detpic32.h>


void delay(int ms){

    resetCoreTimer();
    while(readCoreTimer()< 20000 * ms);

}


void send2Leds(char counter){

    LATE = (LATE & 0xFF00) | counter;

}


int main()
{
    
    //leds
    TRISE = TRISE & 0xFF00;
    TRISB = TRISB | 0x000F;

    //counter
    int counter;
    counter = (PORTB & 0x000F);
    send2Leds(counter);

    while (1)
    {
        if(counter == 0b11111111){
            counter = (PORTB & 0x000F);
        }
        counter = (counter << 1) +1; 
        send2Leds(counter);
        delay(1000);
    }
    

    return 0;
}