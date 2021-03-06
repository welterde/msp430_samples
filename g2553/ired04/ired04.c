
#define WDTCTL     (*((volatile unsigned short *)0x0120))

#define CALBC1_1MHZ (*((volatile unsigned char *)0x10FF))
#define CALDCO_1MHZ (*((volatile unsigned char *)0x10FE))
#define CALBC1_8MHZ (*((volatile unsigned char *)0x10FD))
#define CALDCO_8MHZ (*((volatile unsigned char *)0x10FC))
#define CALBC1_12MHZ (*((volatile unsigned char *)0x10FB))
#define CALDCO_12MHZ (*((volatile unsigned char *)0x10FA))
#define CALBC1_16MHZ (*((volatile unsigned char *)0x10F9))
#define CALDCO_16MHZ (*((volatile unsigned char *)0x10F8))

#define DCOCTL  (*((volatile unsigned char *)0x56))
#define BCSCTL1 (*((volatile unsigned char *)0x57))
#define BCSCTL2 (*((volatile unsigned char *)0x58))

#define TACTL   (*((volatile unsigned short *)0x0160))
#define TAR     (*((volatile unsigned short *)0x0170))
#define TACCR0  (*((volatile unsigned short *)0x0172))
#define TACCTL0 (*((volatile unsigned short *)0x0162))

#define UCA0CTL0    (*((volatile unsigned char *)0x060 ))
#define UCA0CTL1    (*((volatile unsigned char *)0x061 ))
#define UCA0BR0     (*((volatile unsigned char *)0x062 ))
#define UCA0BR1     (*((volatile unsigned char *)0x063 ))
#define UCA0MCTL    (*((volatile unsigned char *)0x064 ))
#define UCA0STAT    (*((volatile unsigned char *)0x065 ))
#define UCA0RXBUF   (*((volatile unsigned char *)0x066 ))
#define UCA0TXBUF   (*((volatile unsigned char *)0x067 ))
#define UCA0ABCTL   (*((volatile unsigned char *)0x05D ))
#define UCA0IRTCTL  (*((volatile unsigned char *)0x05E ))
#define UCA0IRRCTL  (*((volatile unsigned char *)0x05F ))
#define IE2         (*((volatile unsigned char *)0x001 ))
#define IFG2        (*((volatile unsigned char *)0x003 ))



#define P1IN  (*((volatile unsigned char *)0x0020))
#define P1OUT (*((volatile unsigned char *)0x0021))
#define P1DIR (*((volatile unsigned char *)0x0022))
#define P1REN (*((volatile unsigned char *)0x0027))
#define P1SEL (*((volatile unsigned char *)0x0026))
#define P1SEL2 (*((volatile unsigned char *)0x0041))

//-------------------------------------------------------------------
void uart_init ( void )
{
    BCSCTL2 &=~0x06;

    UCA0CTL0 = 0x00;
    UCA0CTL1 = 0xC0;
    UCA0BR0  = 0x08;
    UCA0BR1  = 0x00;
    UCA0MCTL = 0xB1;

    UCA0STAT  = 0x00;
    UCA0ABCTL = 0x00;
    IE2       = 0x00;

    P1SEL  |= 0x06;
    P1SEL2 |= 0x06;
}
//-------------------------------------------------------------------
void uart_putc ( unsigned char c )
{
    while(UCA0STAT&0x01) continue;
    UCA0TXBUF=c;
}
//-------------------------------------------------------------------
void hexstring ( unsigned short d )
{
    //unsigned short ra;
    unsigned short rb;
    unsigned short rc;

    rb=16;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_putc(rc);
        if(rb==0) break;
    }
    uart_putc(0x0D);
    uart_putc(0x0A);
}
//-------------------------------------------------------------------
void hexstrings ( unsigned short d )
{
    //unsigned short ra;
    unsigned short rb;
    unsigned short rc;

    rb=16;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_putc(rc);
        if(rb==0) break;
    }
    uart_putc(0x20);
}
//-------------------------------------------------------------------
//0x039C 0x04CF 924 1231 midpoint 538us

#define HIMIN 900
#define HIMAX 1300

// 600us 1200 0x048C 0x059B  1164 1435
//1200us 2400 0x08FE 0x09D2  2302 2514

#define SHORTMIN 1100
#define SHORTMAX 1500
#define LONGMIN 2200
#define LONGMAX 2600
//-------------------------------------------------------------------
void notmain ( void )
{
    unsigned short sb;
    unsigned short a,b,c;
    unsigned short code;


    WDTCTL = 0x5A80;

    // use calibrated clock
    DCOCTL = 0x00;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    uart_init();
    hexstring(0x1234);

    // make p1.0 and p1.6 outputs LEDs
    P1DIR |= 0x41;
    P1OUT &= ~0x41;
    // make p1.4 an input
    P1DIR&=~0x10;

    TACTL = 0x02E0;

    while(1)
    {
        while(P1IN&0x10) continue; //to LOW
        a=TAR;
        while(1) if(P1IN&0x10) break; //to HIGH
        b=TAR;
        c=b-a;
        if(c<(4800-480)) continue;
        if(c>(4800+480)) continue;
        code=0;
        for(sb=0;sb<12;sb++)
        {
            while(P1IN&0x10) continue; //to LOW
            a=TAR;
            c=a-b;
            if(c<HIMIN) break;
            if(c>HIMAX) break;

            while(1) if(P1IN&0x10) break; //to HIGH
            b=TAR;
            c=b-a;
            if(c<SHORTMIN) break;
            if(c>LONGMAX) break;
            code<<=1;
            if(c<SHORTMAX)
            {
                //code|=0;
            }
            else
            {
                if(c>LONGMIN)
                {
                    code|=1;
                }
                else
                {
                    break;
                }
            }
        }
        if(sb<12) continue;


        hexstring(code);

    }

}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
