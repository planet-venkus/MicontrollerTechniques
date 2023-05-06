/*
 * Name: Subramanyam Srinivasa Venkat
 * Matriculation No: 4967444
 * Exercise No: 05
 */
#include <templateEMP.h>


int melodybtn = 0;
int pauseflag = 0;
int tunecache = 0;
int isbreakflag = 0;
/**
 * main.c
 */
void main(void)
{
	initMSP();                                  //Initialize MSP
	
	while(1)
	{
	   //initialize Piezo Buzzer as Input,i.e, to detect the vibration and play Melody 1 or Melody 2
	    P3SEL &= 0x00;
	    P3SEL2 &= 0X00;
	    P3DIR &= ~BIT6 ;                        // P3 .6 as input
	    P3REN |= BIT6;                          //Enable Pull-up resistor
	    P3OUT |= BIT6;                          //Set to Pull-up for P3.6

	    if(((P3IN & BIT6) == 0))
	    {
	        __delay_cycles(50000);              //Debouncing Operation 50ms
	        melodybtn = 1;
	        long int cnt = 30000;
	        while(cnt > 0)
	        {
	            cnt--;
	            if((P3IN & BIT6)== 0)           //P3.6 Vibrational input
	            {
	                __delay_cycles(50000);      //Debouncing operation 50ms
	                melodybtn++;
	            }
	        }

	        TA0CCTL2 = OUTMOD_5;

	        if(melodybtn <= 2)
	        {
	            playNote(melodybtn);
	            melodybtn=0;
	        }
	    }

	    InterruptPort1Init();                   //Initialize PB5 and PB6
	    while(melodybtn > 0)
	    {
	        __delay_cycles(1000000);
	        if((melodybtn == 1 || melodybtn == 2))
	        {
	            TA0CCTL2 = OUTMOD_5;
	            playNote(melodybtn);            //Play Melody 1 or Melody 2
	        }

	        melodybtn = 0;
	    }

	    if(pauseflag == 0 &&  isbreakflag == 1)
	    {
	        isbreakflag = 0;
	        resumePlayNote(tunecache);          //Resume Playing Melody
	    }

	}
}

void InterruptPort1Init()
{/*Input Params: None
* Returns: None
* Desc: Initialize P1.3 and P1.4
*/
    P1DIR &= ~ BIT3;                            // Set as input
    P1REN |= BIT3;                              // Enable pull - resistors for pin P1.3
    P1OUT |= BIT3;                              // Set to pull -up for pin P1.3
    P1IE |= BIT3;                               // Enable interrupt for pin P1.3
    P1IES |= BIT3;                              // High / Low - Edge for pin P1.3
    P1IFG &= ~ BIT3;                            // clear interrupt flag for pin P1.3

    P1DIR &= ~ BIT4;                            // Set as input
    P1REN |= BIT4;                              // Enable pull - resistors for pin P1.4
    P1OUT |= BIT4;                              // Set to pull -up for pin P1.4
    P1IE |= BIT4;                               // Enable interrupt for pin P1.4
    P1IES |= BIT4;                              // High /Low - Edge for pin P1.4
    P1IFG &= ~ BIT4;                            // clear interrupt flag for pin P1.4

    P3DIR |= BIT6;                             // P3 .6 output (PB5 and PB6 control the operation of Jukebox)
    P3SEL |= BIT6;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void)
{
    if ((P1IN & BIT3) == 0)
    {
        __delay_cycles(200000);
        melodybtn++;                            //If PB5 is pressed once play melody 1 and if PB5 is pressed twice play melody 2
    }
    if((P1IN & BIT4) == 0)
    {
        __delay_cycles(200000);

        if(pauseflag == 0)
        {
            pauseflag = 1;                      //PB6 is pressed to pause
        }
        else if(pauseflag == 1)
        {
            pauseflag = 0;                      //PB6 is presed to resume
        }
    }

    P1IFG &= ~BIT4;                             //clear interrupt flag for P1.4
    P1IFG &= ~BIT3;                             //clear interrupt flag for P1.3
}

void playNote(int presscounter)
/* Input Params: Integer, Number of Press / Vibrational Input
 * Returns: None
 * Desc: Play Melody 1 / Melody 2
 */
{
    P1IE &= ~BIT3;                              // Disable interrupt for pin P1.3
    P3DIR |= BIT6;                              // P3 .6 output
    P3SEL |= BIT6;
    P3REN &= ~BIT6;                             // disable Pull up
    TA0CCTL2 = OUTMOD_3;                        // CCR2 set / reset
    int melodies[12] = {440, 440, 440, 349, 523, 440, 500, 523, 440, 349, 475, 523};
    int counter;

    if((presscounter==1))
    {
        for(counter=0; counter<6; counter++)
        {
            TA0CCR0 = melodies[counter];
            TA0CCR2 = melodies[counter] / 2;    //CCR2 PWM duty cycle (50%)
            TA0CTL = TASSEL_2 + MC_1;
            __delay_cycles(500000);
            if(pauseflag == 1)
            {
                tunecache = counter;
                TA0CCTL2 = OUTMOD_5;
                pauseflag = 1;
                isbreakflag = 1;
                break;
            }

            if(counter==5)
            {
                TA0CCTL2 = OUTMOD_5 ;
            }
        }
    }
    else if((presscounter==2))
    {
        for (counter = 6; counter < 12; counter ++)
        {
            TA0CCR0 = melodies[counter];
            TA0CCR2 = melodies[counter]/2;
            TA0CTL = TASSEL_2 + MC_1 ;

            if(pauseflag == 1)
            {
                tunecache = counter;
                TA0CCTL2 = OUTMOD_5;
                pauseflag = 1;
                isbreakflag = 1;
                break;
            }

            __delay_cycles(500000);

            if(counter== 11)
            {
                TA0CCTL2 = OUTMOD_5 ;
            }

        }
    }

    P1IE |= BIT3;                               // Enable interrupt for pin P1.3
}

void resumePlayNote(int strCnt)
{/* Input Params: Integer, Count where to Resume the Playback Melody
*   Returns: None
*   Desc: Resume / Pause Playback melody
*/
    P1IE &= ~BIT3;                              // Disable interrupt for pin P1.3
    P3DIR |= BIT6 ;                             // P3 .6 output
    P3SEL |= BIT6 ;
    P3REN &= ~BIT6;                             // disable Pull up
    TA0CCTL2 = OUTMOD_3 ;                       // CCR2 set / reset
    int melodies[12] = {440, 440, 440, 349, 523, 440, 500, 523, 440, 349, 475, 523};
    int counter;
    if(strCnt < 6)
    {
        for (counter = strCnt; counter < 6; counter ++)
        {
            TA0CCR0 = melodies[counter];
            TA0CCR2 = melodies[counter]/2;           // CCR2 PWM duty cycle (50 %)
            TA0CTL = TASSEL_2 + MC_1;
            if(pauseflag==1)
            {
                tunecache = counter;
                TA0CCTL2 = OUTMOD_5;
                pauseflag = 1;
                isbreakflag = 1;
                break;
            }

            __delay_cycles(500000);

            if(counter == 5)
            {
                TA0CCTL2 = OUTMOD_5;
            }
        }
    }
    else if(strCnt >= 6)
    {
        for(counter = strCnt; counter < 12; counter ++)
        {
            TA0CCR0 = melodies[counter];
            TA0CCR2 = melodies[counter]/2;           // CCR2 PWM duty cycle (50 %)
            TA0CTL = TASSEL_2 + MC_1;

            __delay_cycles(500000);

            if((P1IN & BIT4) == 0)
            {
                tunecache = counter;
                TA0CCTL2 = OUTMOD_5;
                pauseflag = 1;
                isbreakflag = 1;
                break;
            }
            if(counter == 11)
            {
                TA0CCTL2 = OUTMOD_5;
            }
        }
    }

    P1IE |= BIT3;                               // Enable interrupt for pin P1.3
}
