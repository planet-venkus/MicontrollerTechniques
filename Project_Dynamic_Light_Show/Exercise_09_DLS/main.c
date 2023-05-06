#include <msp430.h> 
#include <templateEMP.h>
/* Name: Subramanyam Srinivasa Venkat
 * Exercise No: 09
 * Topic: A Dynamic Light Show
 */
unsigned int OFCount;
int isRW_flag = 0;
int time_counter = 0;
int time_counter_1 = 0;

int is_ldr_used = 0;
int ldrThreshold = 100;                     // TO BE CHANGED
int max_ldr_adc = 255;

int led_effect_counter = 1;
int pwm_led_value = 1000;
int pwm_led_comp = 0;

/**
 * main.c
 */
void main(void)
{
    initMSP();

    Port2Init();

    PosClkEdge();                                 //Positive clock edge for shift register operation

    InitInterrupt();
    //initTimerA();
    initTimerA1();
    //__enable_interrupt();

    while(1){

       ADC10CTL0 = ADC10ON + ADC10SHT_2;
       ADC10AE0 |= BIT7;
       ADC10CTL1 = INCH_7;                      // Select channel 7 as input for the following conversion (s)

       int ldrDC = ADC();

       //serialPrintInt(123);
       ADC10CTL0 = ADC10ON + ADC10SHT_2;
       ADC10AE0 |= BIT4;                     // Enable P1.7 as AD input
       ADC10CTL1 = INCH_4 ;                     // Select channel 4 as input for the following conversion (s)

       //Start ADC conversion
       int potenDC = ADC();


       P3DIR |= BIT6;
       P3SEL |= BIT6;
       P3REN &= ~BIT6;          //Disable Pull up

       LED_PWM_function(is_ldr_used, ldrDC, max_ldr_adc);


        P2OUT |= BIT0 + BIT5 + BIT6;            //Setting SR(2) to Right Shift mode(S1 = Low, S0 = High) and pushing High Bit into the SR
        P2OUT &= ~BIT1;

        PosClkEdge();

        if(potenDC < 500){
            RewindAction(potenDC);
        }
        else{
            ForwardAction(potenDC);
        }
      }
}

int ADC()
{
/* Input Params : None
* Returns : Integer value corresponding to the analog to digital converted value*
*  Func Desc: Analog to Digital converter
*
*/
    ADC10CTL0 |= ENC + ADC10SC;
    while ( ADC10CTL1 & ADC10BUSY );
    int mem = ADC10MEM ;

    return mem;
}

void PosClkEdge()
{/*
*Input Params: None
*Returns: None
*Function Desc: Generates positive edge clock
*/
    P2OUT |= BIT4 ;                             //Positive Edge
    P2OUT &= ~BIT4 ;
}

void initTimerA(void){
    TACCR0 = 10000; //Initially, Stop the Timer
    TACTL = TASSEL_2  + MC_1; //+ ID_3
    CCTL0 = CCIE; //Enable interrupt for CCR0.
}


//#pragma vector = TIMER0_A0_VECTOR
//__interrupt void Timer_A0(void)
//{
//   if(time_counter > 500){
//    P1OUT ^= BIT5;
//    time_counter = 0;
//   }
//   else
//   {
//       time_counter++;
//   }
//    TACTL &= ~TAIFG;                                // Clear Timer A Interrupt Flag
//}

void initTimerA1(void)
{
    //Timer0_A3 Configuration
    TA1CCR0 = 0;                                     //Initially, Stop the Timer
    TA1CCTL0 |= CCIE;                                //Enable interrupt for CCR0.
    TA1CTL = TASSEL_2 + ID_0 + MC_1;                 //Select SMCLK, SMCLK/1, Up Mode
}

void delayMS(int msecs)
{
    OFCount = 0;                                    //Reset Over-Flow counter
    TA1CCR0 = 1000-1;                               //Start Timer, Compare value for Up Mode to get 1ms delay per loop
    //Total count = TACCR0 + 1. Hence we need to subtract 1.

    while(OFCount <= msecs);
}

//Timer ISR
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer_A1(void)
{
    OFCount++; //Increment Over-Flow Counter
}


void RewindAction(int potDC)
{
    //Pushing HIGH BIT to last LED and doing a left shift operation
    int j;

    P2OUT |= BIT0 + BIT5 + BIT6;
    P2OUT &= ~BIT1;

    PosClkEdge();

    for(j=0; j < 3; j++){
        P2OUT &= ~BIT6;

        PosClkEdge();
    }

    P2OUT |= BIT1;
    P2OUT &= ~BIT0;

    for(j=0; j < 4; j++){
        if(potDC > 400 && potDC <= 500){
             delayMS(2000);
        }
        else if(potDC > 300 && potDC <= 400){
             delayMS(1000);
        }
        else if(potDC > 200 && potDC <= 300){
             delayMS(500);
        }
        else if(potDC > 0 && potDC <= 200){
             delayMS(100);
        }
        //__delay_cycles(250000);
        PosClkEdge();
    }
}

void ForwardAction(int potDC){
    int i;

   if(potDC > 500 && potDC <= 600){
       delayMS(2000);
   }
   else if(potDC > 600 && potDC <= 700){
       delayMS(1000);
   }
   else if(potDC > 700 && potDC <= 800){
       delayMS(500);
   }
   else if(potDC > 800 && potDC < 1024){
       delayMS(100);
   }

    for(i = 1; i <= 4; i++){
        P2OUT &= ~BIT6;
        PosClkEdge();

        if(potDC > 500 && potDC <= 600){
               delayMS(2000);
        }
        else if(potDC > 600 && potDC <= 700){
               delayMS(1000);
        }
        else if(potDC > 700 && potDC <= 800){
               delayMS(500);
        }
        else if(potDC > 800 && potDC < 1024){
               delayMS(100);
        }
     }
}
void LED_PWM_function(int is_ldr_used, int ldr_adc, int max_ldr_adc){
   //TA0CCTL2 = OUTMOD_5;                             // Set output mode to set/reset
    if(pwm_led_comp > 1000){
        pwm_led_comp  = 0;
    }
   int Const_Illumination_Vector[20] = {pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp, pwm_led_comp};
   int Blink_Illumination_Vector[20] = {pwm_led_comp, 0, pwm_led_comp, 0,pwm_led_comp, 0, pwm_led_comp, 0,pwm_led_comp, 0, pwm_led_comp, 0,pwm_led_comp, 0, pwm_led_comp, 0,pwm_led_comp, 0, pwm_led_comp, 0,pwm_led_comp, 0, pwm_led_comp, 0};
   int Breath_Illumination_Vector[22] = {pwm_led_comp/20, pwm_led_comp/18, pwm_led_comp/16, pwm_led_comp/14, pwm_led_comp/12, pwm_led_comp/10, pwm_led_comp/8, pwm_led_comp/6, pwm_led_comp/4, pwm_led_comp/2, pwm_led_comp, pwm_led_comp/2, pwm_led_comp/4, pwm_led_comp/6, pwm_led_comp/8, pwm_led_comp/10, pwm_led_comp/12, pwm_led_comp/14, pwm_led_comp/16, pwm_led_comp/18, pwm_led_comp/20};

   if(led_effect_counter == 1){
       int counter = 0;
       for(counter = 0; counter <= 20; counter++)
       {
           TA0CCTL2 = OUTMOD_3;
           TA0CTL = TASSEL_2 + MC_1 + ID_3;// CCR2 set / reset
           TA0CCR0 = pwm_led_value;
           TA0CCR2 = Const_Illumination_Vector[counter];
           __delay_cycles(200000);
       }
   }
   else if(led_effect_counter == 2){
       int counter = 0;
       for(counter = 0; counter <= 20; counter++)
       {
           TA0CCTL2 = OUTMOD_3;
           TA0CTL = TASSEL_2 + MC_1 + ID_3;// CCR2 set / reset
           TA0CCR0 = pwm_led_value;
           TA0CCR2 = Blink_Illumination_Vector[counter];
           __delay_cycles(200000);
       }
   }
   else if(led_effect_counter == 3){
       int counter = 0;
       for(counter = 0; counter <= 22; counter++)
       {
           TA0CCTL2 = OUTMOD_3;
           TA0CTL = TASSEL_2 + MC_1 + ID_3;// CCR2 set / reset
           TA0CCR0 = pwm_led_value;
           TA0CCR2 = Breath_Illumination_Vector[counter];
           __delay_cycles(200000);
       }

   }
}

void Port2Init(){
    P2SEL = 0x00;                           //Function Select Register -- P2SEL
    P2SEL2 = 0x00;                          //I/O function is selected

    P2DIR |= 0x7F;                          //Setting P2.0, P2.1, P2.2, P2.3, P2.4, P2.5, P2.6 direction as output and P2.7 direction as input
    P2DIR &= ~BIT7;                         //P2.7 receives output of SR1(QD(1)) as input (NOT Implemented!!!)

    P2OUT &= ~BIT5;                        // Clear / reset the shift register , i.e. turn all LEDs off.
    P2OUT |= BIT5 ;                         //Stop the reset mode (/CLR == 1)
}

void InitInterrupt(void){
    P1DIR &= ~BIT3;             // Set as Input
    P1REN |= BIT3;              // Enable pull-resistors
    P1OUT |= BIT3;              // Set to pull-up
    P1IE |= BIT3;               // Enable interrupt
    P1IES |= BIT3;              // High/low-Edge
    P1IFG &= ~BIT3;             // Clear interrupt flag

    P1DIR &= ~BIT0;             // Set as Input
    P1REN |= BIT0;              // Enable pull-resistors
    P1OUT |= BIT0;              // Set to pull-up
    P1IE |= BIT0;               // Enable interrupt
    P1IES |= BIT0;              // High/low-Edge
    P1IFG &= ~BIT0;             // Clear interrupt flag
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void){

    //Interrupt Service routine for PB5 connected to P1.0
    if((P1IN & BIT0) ==0 && (P1IN & BIT3)!= 0){
        __delay_cycles(50000);          //delay for debouncing operation

        //P1OUT ^= BIT5;            //(FOR TESTING)

        led_effect_counter++;               //Default value executes constant illumination

        if(led_effect_counter >= 4){
            led_effect_counter = 1;
        }
    }

    //Interrupt Service routine for PB6 connected to P1.3
    if((P1IN & BIT3)==0 && (P1IN & BIT0)!=0){
        __delay_cycles(50000);           //delay for debouncing operation

        //pwm_led_comp = pwm_led_comp + 100;
//        P1OUT ^= BIT6;            (FOR TESTING)
//        P1OUT ^= BIT5;            (FOR TESTING)
        pwm_led_comp = pwm_led_comp + 100;   //When PB6 is pressed,  the brightness is modified
    }

    P1IFG &= ~(BIT3 | BIT0);
}
