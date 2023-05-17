#include <detpic32.h>
#define msVal 20000

void delay(unsigned int);

int main(void)
{
	TRISEbits.TRISE6 = 0;
	TRISEbits.TRISE5 = 0;
	TRISEbits.TRISE4 = 0;
	TRISEbits.TRISE3 = 0;
	
	int counter = 0;
	while(1)
	{
		delay(2000);								// Wait 250 ms
		LATE = (LATE & 0x87) | (counter << 3);		// 1000 0111
		counter++;
		if (counter > 9){
			counter = 0;
		}
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
