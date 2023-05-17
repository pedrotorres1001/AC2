#include <detpic32.h>

#define msVal 20000

void delay(unsigned int);

int main(void)
{
	TRISCbits.TRISC14 = 0;
	while(1)
	{
		delay(500); 							// Wait 0.5s
		LATCbits.LATC14 = !LATCbits.LATC14;		// Toggle RC14 port value
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
