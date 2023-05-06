/*
 * Name: Subramanyam Srinivasa Venkat
 * Matr No: 4967444
 * Exercise No: 06
 *
 * Connections:
 * P3.1 is connected to X5(REL_STAT)
 * P1.4 is connected to X7(COMPU_OUT)
 * P3.6 is connected to Pin1(Links) of COMP4
 * X1(BUZZER) is connected to Pin2(Mitte) of COMP4
 * X2(DAC_IN) is connected to Pin3(Rechts) of COMP4
 */
#include <templateEMP.h>

int melodybtn = 0;                                  //To play Melody 1 or Melody 2


/**
 * main.c
 */
void main(void)
{
    initMSP();                                      //Initialize MSP430 board
    while(1)
    {
        PortInit();
        while(melodybtn > 0)
        {
            __delay_cycles(1000000);
            if(melodybtn <= 2)
            {
                playNote(melodybtn);                //Play melody 1 or melody 2
                melodybtn = 0;
            }
            else
            {
                melodybtn = 0;
            }
        }
    }

}

void PortInit()
/* Input Params: None
 * Returns: None
 * Desc: Initializes P1.4 as input, P3.6 and P3.1 as output
 */
{
    P1DIR &= ~BIT4;                                 //Set P1.4 as input
    P1OUT |= BIT4;                                  //Set tp Pull-up for pin P1.4
    P1REN |= BIT4;                                  //Enable Pull-up resistor for P1.4
    P1IE |= BIT4;                                   //Enable Interrupt for P1.4
    P1IES |= BIT4;                                  //High /Low - Edge for P1.4
    P1IFG &= ~BIT4;                                 //Clear interrupt flag for P1.4

    P3DIR |= BIT6;                                  //Set P3.6 as Output
    P3SEL |= BIT6;
    P3REN &= ~BIT6;                                 //Pull-up disabled

    P3DIR |= BIT1;                                  //Set P3.1 as Output
    P3OUT &= ~BIT1;                                 //Setting Relay to Rechts
}

#pragma vector = PORT1_VECTOR;
__interrupt void Port_1(void)
{
    if((P1IN & BIT4) == 0)
    {
        __delay_cycles(200000);                     //Debouncing delay
        melodybtn++;
    }

    P1IFG &= ~BIT4;                                 //Clear interrupt flag for P1.4
}

void playNote(int presscounter)
/* Input Params: Integer - presscount
 * Returns: None
 * Desc: Play Melody 1 or 2 based on presscount value.
 */
{
    P3OUT |= BIT1;                                  //Set P3.1 as Output
    TA0CCTL2 = OUTMOD_3;                            //CCR2 in set/reset mode
    int melodies[12] = {440, 440, 440, 349, 523, 440, 500, 523, 440, 349, 475, 523};
    int counter;

    if((presscounter == 1))
    {
        for(counter = 0; counter < 6; counter++)
        {
            TA0CCR0 = melodies[counter];
            TA0CCR2 = melodies[counter] / 2;        // CCR2 PWM duty cycle 50%
            TA0CTL = TASSEL_2 + MC_1;

            __delay_cycles(500000);

            if(counter == 5)
            {
                TA0CCTL2 = OUTMOD_5;                // reset CCR2
            }

        }
    }
    else if((presscounter == 2))
    {
        for(counter = 6; counter < 12; counter++)
        {
            TA0CCR0 = melodies[counter];
            TA0CCR2 = melodies[counter] / 2;        // CCR2 PWM duty cycle 50%
            TA0CTL = TASSEL_2 + MC_1;

            __delay_cycles(500000);

            if(counter == 11)
            {
                TA0CCTL2 = OUTMOD_5;                // reset CCR2
            }
        }
    }
}
