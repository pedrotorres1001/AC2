#include <detpic32.h>
#define msVal 20000

void delay(unsigned int); 

int main(void)
{
unsigned char segment;
	int i;
	LATDbits.LATD5 = 0;
	LATDbits.LATD6 = 1;// enable display low (RD5) and disable display high (RD6)
	TRISB = 0x80FF;// configure RB8-RB14 as outputs
	TRISD = 0xFF9F;// configure RD5-RD6 as outputs
	while(1)
	{
		segment = 1;
		for(i=0; i < 7; i++)
		{
			LATB = (LATB & 0x80FF) | (segment << 8) ; // 1000 0000 1111 111  send "segment" value to display
			delay(500);// wait 0.5 second
			segment = segment << 1;
		}
			LATDbits.LATD5 = !LATDbits.LATD5;
			LATDbits.LATD6 = !LATDbits.LATD6;// toggle display selection
	}
	return 0;
}

void delay (unsigned int ms)
{
    unsigned int wait;

    resetCoreTimer();
    wait = msVal * ms;
    while (readCoreTimer() < wait);
}
