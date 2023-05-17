#include <detpic32.h>

void send2Displays(unsigned char choice)
{

    static const char numb[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67,
                                0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    // 1000111

    // putChar(choice);
    static int displayFlag = 0;

    if (displayFlag == 0)
    {

        // Colocar OR em ambos
        LATD = (LATD & 0xFF9F) | 0xFFBF; // 1101 1111

        LATB = (LATB & 0x80FF) | (numb[choice & 0x0F] << 8);

        displayFlag = 1;
    }
    else
    {
        LATD = (LATD & 0xFF9F) | 0xFFDF; // 1011 1111
        LATB = (LATB & 0x80FF) | (numb[choice >> 4] << 8);
        displayFlag = 0;
    }
}
void delay(int ms)
{

    resetCoreTimer();
    while (readCoreTimer() < 20000 * ms)
        ;
}

void hertz(char mandar)
{
    int i;
    if (mandar = 0xFF)
    {
        {
            send2Displays(mandar);
            delay(10);
            i = (i + 1) % 100;
            if (i == 0)
                LATD = LATD & 0xFF9F;
                return;
        }
    }
    else
    {
        while (1)
        {
            send2Displays(mandar);
            delay(10);
            i = (i + 1) % 100;
            if (i == 0)
                return;
        }
    }
}



int main()
{
    // Outputs

    TRISB = TRISB & 0x80FF; // 1000 0000 1111 1111
    TRISD = TRISD & 0xFF9F;

    char a;
    while (1)
    {

        a = getChar();
        // putChar(a);
        switch (a)
        {
        case '0':
            hertz(0x00);
            break;
        case '1':
            hertz(0x01);
            break;
        case '2':
            hertz(0x02);
            break;
        case '3':
            hertz(0x03);
            break;
        default:
            hertz(0xFF);
            break;
        }
    }
    return 0;
}
