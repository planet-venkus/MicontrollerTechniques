/* Name: Subramanyam Srinivasa Venkat
 * Matriculation No: 4967444
 * Exercise No: 07
 */
#include <templateEMP.h>
#define Task_2

/**
 * main.c
 */

void InterruptPort1Init();
void NtcADC();
void PosClk();
void LEDGlow(int reading);
void HeatRange();
void TimerAInit();

volatile int pb5press;
volatile int NTCVoltage;
volatile int timeCounter;
volatile int cnt;

int main(void)
{
    initMSP();

    pb5press = 0;
    NTCVoltage = 0;
    timeCounter = 0;

    BCSCTL3 |= LFXT1S1;                                          //Select VLOCLK (12khz, Time period t ACLK * 2^13)

    WDTCTL = WDTIS0 | WDTCNTCL | WDTPW | WDTSSEL;    //Select ACLK and divide by 8192. For count = 65535, Time Period = 5.5s

    #ifdef Task_2
    WDTCTL = WDTPW + WDTSSEL + WDTTMSEL + WDTCNTCL;           //Select ACLK and divide by 8192. For count = 65535, Time Period ~ 21.9s
    #endif

    IE1 |= WDTIFG;                                               //enable the interrupt
    IFG1 &= ~WDTIFG;

    P1DIR |= BIT0;

    InterruptPort1Init();

    P3OUT &= ~BIT4;

    TimerAInit();

    while(1)
    {
        if(pb5press==0)
        {
            P1OUT |= BIT0;

            __delay_cycles(125000);

            P1OUT &= ~BIT0;
            __delay_cycles(125000);

            HeatRange();

            WDTCTL = WDTIS0 | WDTCNTCL | WDTPW | WDTSSEL;
        }
        if(pb5press==1)
        {
            P3OUT |= BIT4;
            while(1);
        }
    }

}

void InterruptPort1Init()
/* Input Params: None
 * Returns: None
 * Desc: Initialize P1.3 as Input and P3.4 as Output
 */
{
    P1DIR &= ~BIT3;                                             //Set P1.3 as input
    P1REN |= BIT3;                                              //Enable pull-up resistor for P1.3
    P1OUT |= BIT3;                                              //Set pull up resistor for P1.3
    P1IE |= BIT3;                                               //Enable interrupt for P1.3
    P1IES |= BIT3;                                              //High/Low - Edge for P1.3
    P1IFG &= ~BIT3;                                             //Clear interrupt flag for P1.3

    P3DIR |= BIT4;
    P3OUT &= ~BIT4;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void)
{
    if((P1IN & BIT3) == 0)                                      // Check for PB5 button press.
    {
        __delay_cycles(50000);
        pb5press = 1;
    }
    P1IFG &= ~BIT3;                                             // clear interrupt flag for pin P1.3
}

void NtcADC()
{
    ADC10CTL0 = ADC10ON + ADC10SHT_2 ;                          // Turn ADC on; use 16 clocks as sample & hold time ( see p. 559 in the user guide )
    ADC10AE0 |= BIT5;                                           // Enable P1.5 as AD input
    ADC10CTL1 = INCH_5 ;                                        // Select channel 5 as input for the following conversion (s)

    ADC10CTL0 |= ENC + ADC10SC ;                                // Start conversion
    while ( ADC10CTL1 & ADC10BUSY );                            // Wait until result is ready

    int mem1 = ADC10MEM ;

    NTCVoltage = mem1;

    LEDGlow(mem1);

    serialPrintInt(mem1);

    ADC10CTL0 &= ~BIT5;                                         // Turn off ADC
}

void PosClk()
{
/* Input Params: None
 * Returns: None
 * Desc: Raising Edge of Clock pulse
 */
    P2OUT |= BIT4;
    P2OUT &= ~BIT4;
}

void LEDGlow(int reading)
{
/* Desc: Measures ADC converted values and LEDs D1 to D4 are turned accordingly
 *
 * ADC Op           |         <375        |  375 - 420  |  420 - 470  |  470 - 530  |  530 - 610   |  > 610
 * OnLEDs           |        No Led       |     D1      |   D1 D2     |   D1 D2 D3  | D1 D2 D3 D4  | D1 D2 D3 D4 Red
 */
    P2SEL = 0x00;                                               //Select I/O operation
    P2SEL2 = 0x00;

    P2DIR = 0xFF;

    P2OUT &= ~BIT5;                                             //Reset all SR to 0. /CLR
    PosClk();

    P2OUT |= BIT5;
    PosClk();

    int ledcnt = 0;

    if(reading<375)
    {
        ledcnt = 0;
    }
    else if (reading >= 375 && reading < 420)
    {
        ledcnt = 1;
    }
    else if (reading >= 420 && reading < 470)
    {
        ledcnt = 2;
    }
    else if (reading >= 470 && reading < 530)
    {
        ledcnt = 3;
    }
    else if (reading >= 530 && reading < 610)
    {
        ledcnt = 4;
    }
    else if (reading >= 610)
    {
        ledcnt = 5;
    }

    //The Number of LEDs to glow based on ledcnt value
    if(ledcnt == 0)
    {
        P2OUT &= ~BIT5;
        PosClk();
        P2OUT |= BIT5;
        P2OUT &= ~BIT6;                                             // Next Bit shifted is 0
        PosClk();
    }
    else if(ledcnt > 0)
    {
        if(ledcnt < 5)
        {
            P3DIR |= BIT2;                                          //P3.2(Red LED) is set as output
            P3OUT &= ~BIT2;                                         //Red LED is OFF
        }
        else if(ledcnt == 5)
        {
            P3DIR |= BIT2;
            P3OUT |= BIT2;                                          //RED Led connected to P3.2 is ON
            ledcnt--;
        }

        P2OUT &= ~BIT5;
        PosClk();

        P2OUT |= BIT0 + BIT5 + BIT6;                                // SR operates in Right Shift Mode
        P2OUT &= ~BIT1;

        while(ledcnt > 0)
        {
            PosClk();
            ledcnt--;                                               // LED are turned ON based on ledcnt value.
        }
    }
}

void HeatRange()
{
/* Input Param: None
 * Returns: None
 * Desc. The heater conencted to P3.4 is configured to operate in ranges 3 & 4
 */
    if(NTCVoltage < 450)
    {
        P3OUT |= BIT4;
    }
    else if(NTCVoltage >= 550)
    {
        P3OUT &= ~BIT4;
    }
}

void TimerAInit()
{
    TACCR0 = 62500;                                 // CCR0 interrupt enabled, 125000/62500 = 2s

    TACTL = TASSEL_2 + MC_1 + ID_3 + TAIE;          // SMCLK/8, upmode
    CCTL0 = CCIE;

    TACCR1 = 62500;                                 // CCR1 interrupt enabled, 125000/62500 = 2s
    CCTL1 = CCIE;

    __enable_interrupt();                           // set GIE in SR
}

// WatchDog Timer interrupt service routine
#pragma vector = WDT_VECTOR
__interrupt void WdtTimer(void)
{
    cnt++;
    if((pb5press == 1) && (cnt=6))
    {
        WDTCTL = WDTNMI;
        cnt = 0;
    }

    IFG1 &= ~WDTIFG;                                //Clear WatchDog Timer Interrupt Flag
}

// Timer A0 interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0(void)
{

    TACTL &= ~TAIFG;                                // Clear Timer A Interrupt Flag
}

// Timer A1 interrupt service routine
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1(void)
{
    switch(TA0IV)
    {
    case 2:
    {
        timeCounter++;
        if(timeCounter==4)
        {
            NtcADC();
            timeCounter = 0;
        }
    }
    break;
    }
}
