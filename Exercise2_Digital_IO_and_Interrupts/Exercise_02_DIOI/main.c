/*
 * Name: Subramanyam Srinivasa Venkat
 * Matriculation No: 4967444
 * Date of submission: 13 Nov 2021
 * Description of exercise task: The buttons PB5 and PB6 are control buttons which change the state of D5(Green), D6(Red),
 * D7(Blue) and D9(Yellow) LEDs. Polling and Interrupts(ISR) is also implemented.
 */



#include <msp430.h> 
#include <templateEMP.h>

#define polling
//#define PB5 BIT3


void initializeInterrupt();                    //Initializing Interrupt function
void setRedYellow();

void initInterrupt(){
    /* Input Params: None
     * Return Value: None
     * Description: This method is used to initialize PB5 as interrupt
     */

        P1DIR &= ~BIT3;                         // Set as Input
        P1REN |= BIT3;                          // Enable pull-resistors
        P1OUT |= BIT3;                          // Set to pull-up
        P1IE |= BIT3;                           // Enable interrupt
        P1IES |= BIT3;                          // High/low-Edge
        P1IFG &= ~BIT3;                         // Clear interrupt flag
}

void setRedYellow(){
    /* Input Params: None
     * Return Value: None
     * Description: Function to set and reset the RED and YELLOW leds. Controlled by using PB5 and PB6 buttons.
     */
    if((P1IN & BIT3) == 0)                      //Condition when PB5(P1.3): 1 and PB6(P1.4): 0/1
    {
        __delay_cycles(50000);

        if((P1IN & BIT4) != 0)                  //Condition when PB5(P1.3): 1 and PB6(P1.4): 0
        {
           P1OUT |= BIT5;                       //Turn ON RED(D6, P1.5)
           P1OUT &= ~BIT7;                      //Turn OFF YELLOW(D9, P1.7)
           __delay_cycles(250000);              //Delay of 250ms ,i.e, RED led is ON for 250ms and Yellow is OFF for 250ms
           P1OUT &= ~BIT5;                      //Turn OFF RED(D6, P1.5)
           P1OUT |= BIT7;                       //Turn ON YELLOW(D9, P1.7)
           //__delay_cycles(250000);

        }
        else                                    //Condition when PB5(1.3): 1 and PB6(1.4): 1
        {
           P1OUT |= BIT7;                       //Turn ON YELLOW(D9, P1.7)
           P1OUT &= ~BIT5;                      //Turn OFF RED(D6, P1.5)
        }
    }
    else                                        //Condition when PB5(1.3): 0 and PB6: 0/1
    {
        P1OUT |= BIT7;                          //Turn ON YELLOW(D9, P1.7)
        P1OUT &= ~BIT5;                         //Turn OFF RED(D6, P1.5)

    }

}

int main(void)
{
    initMSP();                                  //Initializing MSP. Method used from <templateEMP.h>

    P1SEL == 0x00;                              //Function Select Register -- P1SEL
    P1SEL == 0x00;                              //I/O function is selected
	
    P1DIR |= (BIT6 | BIT5 | BIT7 | BIT0);       //P1.5(D6), P1.6(D5), P1.0(D7), P1.0(D9) set as output pins

    P1REN |= (BIT4 | BIT3);                     //Enables Pull-up/down resistor for Input P1.4(PB6) and P1.3(PB5)
    P1OUT |= (BIT4 | BIT3);                     //Selecting the Pull-up mode

    #ifdef polling
    initInterrupt();                            //Initialize Interrupt function gets executed as polling is defined
    #endif

    while(1)
    {
        if((P1IN & BIT3) == 0)                  //Condition when PB5: 1 and PB6: 0/1
        {                                       //Condition when P1.3(PB5 button is pressed). As PB5 has pull up register, when pressed P1IN=1 when not pressed
            __delay_cycles(50000);              //adding 50ms delay routine for debouncing(software debouncing)

            if((P1IN & BIT4) != 0)              //Condition when PB5: 1 and PB6: 0
            {
                P1OUT &= ~BIT6;                 //Turn OFF GREEN(D5, P1.6)
                P1OUT |= BIT0;                  //Turn OFF BLUE(D7, P1.0). NOTE: Setting P1.0 is turning OFF the BLUE LED
                setRedYellow();
            }

            #ifndef polling                     //Not executed as Polling is defined
            else                                //Condition when PB5: 1 and PB6: 1 . Will not be executed during polling
            {
                __delay_cycles(50000);          //50ms delay routine for debouncing
                P1OUT |= BIT0;                  //Turn ON BLUE(D7, P1.0)
                P1OUT &= ~BIT6;                 //Turn OFF GREEN(D5, P1.6)
                setRedYellow();
            }
            #endif

        }
        else                                    //enters for states PB5: 0 and PB6: 0/1
        {
            if((P1IN & BIT4) == 0)              //Condition when PB5:0 and PB6: 1
            {
                __delay_cycles(50000);          //adding 50ms delay routine for debouncing
                P1OUT |= BIT6;                  //Turn ON GREEN LED(D5. P1.6)
                P1OUT |= BIT0;                  //Turn OFF BLUE LED(D7, P1.0). NOTE: Setting P1.0 is turning OFF the BLUE LED
                setRedYellow();
            }
            else                                //Condition when PB5: 0 and PB6: 0
            {
                P1OUT &= ~BIT6;                 //Turn OFF GREEN LED(D5, P1.6)
                P1OUT |= BIT0;                  //Turn OFF BLUE LED(D7, P1.0). NOTE: Setting P1.0 is turning OFF the BLUE LED
                setRedYellow();
            }
        }

    }

	return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    if((P1IN & BIT3) == 0)                      //Condition when PB5: 1 and PB6: 0/1
    {
        __delay_cycles(50000);                  //delay of 50ms for debouncing operation
        if((P1IN & BIT4) == 0)                  //Condition when PB5: 1 and PB6: 1
        {
            P1OUT &= ~BIT6;                     //Turn OFF GREEN(D5, P1.6)
            P1OUT &= ~BIT0;                     //Turn ON BLUE(D7, P1.0). NOTE: Clearning P1.0 is turning ON the BLUE LED
            setRedYellow();                     //Function for Red and Yellow LED operation
        }
    }
    else                                        //Condition when PB5: 0 and PB6: 0/1
    {
        if((P1IN & BIT4) == 0)                  //Condition when PB5: 0 and PB6: 1
        {
            P1OUT |= BIT6;                      //Turn ON GREEN(D5, P1.6)
            P1OUT |= BIT0;                      //Turn OFF BLUE(D7, P1.0). NOTE: Setting P1.0 is turning OFF the BLUE LED
            setRedYellow();                     //Function for Red and Yellow LED operation
        }
        else                                    //Condition when PB5: 0 and PB6: 0
        {
            P1OUT &= ~BIT6;                     //Turn OFF GREEN(D5, P1.6)
            P1OUT |= BIT0;                      //Turn OFF BLUE(D7, P1.0). NOTE: Setting P1.0 is turning OFF the BLUE LED
            setRedYellow();                     //Function for Red and Yellow LED operation
        }
    }
    P1IFG &= ~BIT3;                             // Clear interrupt flag for P1.3
}
