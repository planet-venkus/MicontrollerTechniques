/*Name: Subramanyam Srinivasa Venkat
 * Matriculation No: 4967444
 * Exercise No: 04
 */
#include <templateEMP.h>

/**
 * main.c
 */
void main(void)
{
    initMSP();

    P3DIR |= BIT0 + BIT1 + BIT2;              // Setting P3.0, 3.1, 3.2 as output pins

    //ADC10CTL0 = ADC10ON + ADC10SHT_2 ;      // Turn ADC on; use 16 clocks as sample & hold time ( see p. 559 in the user guide )

    //ADC10AE0 |= BIT7 ;                      // Enable P1.7 as AD input

    //ADC10CTL1 = INCH_7 ;                    // Select channel 7 as input for the following conversion (s)

    while(1){

        //This section of code is for Chip colour detection
        ADC10CTL0 = ADC10ON + ADC10SHT_2 ;

        ADC10AE0 |= BIT4;

        ADC10CTL1 = INCH_4;

        ChipColour();

        //This section of code is to glow LEDs based on the Potentiometer values
        ADC10CTL0 &= ~BIT7;                     //Turn OFF ADC

        __delay_cycles(200000);

        ADC10CTL0 = ADC10ON + ADC10SHT_2 ;      // Turn ADC on; use 16 clocks as sample & hold time ( see p. 559 in the user guide )

        ADC10AE0 |= BIT7 ;                      // Enable P1.7 as AD input

        ADC10CTL1 = INCH_7 ;                    // Select channel 7 as input for the following conversion (s)

        //ADC10CTL0 |= ENC + ADC10SC ;            // Start conversion

        //while (ADC10CTL1 & ADC10BUSY);

        //int m1 = ADC10MEM ;

        int m1 = ADC();

        //serialPrintln("HI");

        //serialPrintInt(m1);                     // Print m1 to the serial console

        GlowLED(m1);
    }
}


void PosClk()
{/*
*Input Params: None
*Returns: None
*Function Desc: Generates positive edge clock
*/
    P2OUT |= BIT4 ;                             //Positive Edge
    P2OUT &= ~BIT4 ;
}

void GlowLED(int reading){
/* Input Params: None
 * Returns: None
 * Function Desc: LEDs D1, D2, D3, D4 are turned ON according to different Potentiometer values
 *
 * Potentiometer range:     0 - 204     204 - 409      409 - 614       614 - 818        818 - 2013
 * Turned ON LEDs     :      No LED         D1           D1 D2          D1 D2 D3        D1 D2 D3 D4
 */

    P2SEL = 0x00;
    P2SEL2 = 0x00;
    P2DIR = 0xFF;

    P2DIR &= ~BIT7;                             //Set P2.7 as input to get Serial output from SR1

    P2OUT &= ~BIT5;                            // Clear / reset the shift register , i.e. turn all LEDs off.
    PosClk();

    P2OUT |= BIT5 ;                             // Setting BIT5,i.e, /CLR = 0
    PosClk();

    int cnt=0;

    if(reading > 0 && reading <= 204){
        cnt = 0;
    }
    else if(reading > 204 && reading <= 409){
        //serialPrintInt(reading);
        cnt = 1;
    }
    else if(reading > 409 && reading <= 614){
        cnt = 2;
    }
    else if(reading > 614 && reading <= 818){
        cnt = 3;
    }
    else if(reading > 818 && reading <= 1023){
        cnt = 4;
    }

    //To check the cnt value and glow the number of LEDs equal to cnt
    if(cnt == 0){

        P2OUT &= ~BIT5;
        PosClk();

        P2OUT |= BIT5;                          // SR1 functions as a RIGHT SHIFT register
        P2OUT &= ~BIT6;
        PosClk();
    }
    else if(cnt > 0){

        P2OUT &= ~BIT5;
        PosClk();

        P2OUT |= BIT5 + BIT6;

        P2OUT |= BIT0;
        P2OUT &= ~BIT1;

        while(cnt > 0){
            PosClk();
            cnt--;
        }
    }

}

void ChipColour(){
/* Input Params: None
 * Returns: None
 * Func Desc: LDR is configured to detect different LID colors.
 *
 * LID Colour and range values
 *      RED         GREEN       BLUE       YELLOW       MAGENTA      CYAN        BLACK       WHITE
 *    340-364      245-258    350-369      430-455      340-355     173-187     215-232     439-465
 */
    int NoLidFlag = 1, reading;

    P3OUT = BIT0;                               // RED Led ON
    __delay_cycles(2000000);

    reading = ADC();                            //Analog to Digital converter
    serialPrintInt(reading);

    if(reading > 340 && reading < 364)
    {
        while(serialAvailable()==1);
        serialPrintln("|  Red Lid  |");
        serialFlush();
        NoLidFlag = 0;
    }

    P3OUT = BIT1;                               // Turn ON GREEN Led
    __delay_cycles(2000000);

    reading = ADC();
    serialPrintInt(reading);
    if(reading > 245 && reading < 258)
    {
        while(serialAvailable() == 1);
        serialPrintln("|  Green Lid  |");
        serialFlush();
        NoLidFlag = 0;
    }

    P3OUT = BIT2;                               // BLUE Led ON
    __delay_cycles(2000000);

    reading = ADC();
    serialPrintInt(reading);
    if(reading >  350 && reading < 369)
    {
        while(serialAvailable() == 1);
        serialPrintln("|  Blue Lid  |");
        serialFlush();
        NoLidFlag = 0;
    }

    P3OUT = BIT0 + BIT1;
    __delay_cycles(2000000);

    reading = ADC();
    serialPrintInt(reading);
    if(reading > 430 && reading < 455)
    {
        while(serialAvailable() == 1);
        serialPrintln("|  Yellow Lid  |");
        serialFlush();
        NoLidFlag = 0;
    }

    P3OUT = BIT0 + BIT2;
    __delay_cycles(2000000);

    reading = ADC();
    serialPrintInt(reading);
    if(reading > 340 && reading < 355)
    {
        while(serialAvailable() == 1);
        serialPrintln("|  Magenta Lid  |");
        serialFlush();
        NoLidFlag = 0;
    }

    P3OUT = BIT1 + BIT2;
    __delay_cycles(2000000);

    reading = ADC();
    serialPrintInt(reading);
    if(reading > 173 && reading < 187)
    {
        while(serialAvailable() == 1);
        serialPrintln("|  Cyan Lid  |");
        serialFlush();
        NoLidFlag = 0;
    }

    P3OUT = BIT1 + BIT2 + BIT0;
    __delay_cycles(2000000);
    reading = ADC();
    serialPrintInt(reading);
    if(reading > 439 && reading < 465)
    {
        while(serialAvailable() == 1);
        serialPrintln("|  White Lid  |");
        serialFlush();
        NoLidFlag = 0;
    }
    else if(reading > 215 && reading < 232)
    {
        while(serialAvailable() == 1);
        serialPrintln("|  Black Lid  |");
        serialFlush();
        NoLidFlag = 0;
    }
    if(NoLidFlag==1)
    {
        while(serialAvailable() == 1);
        serialPrintln("|  No Lid  |");
        serialFlush();
    }
}

int ADC(){
/* Input Params : None
 * Returns : Integer value corresponding to the analog to digital converted value
 * Func Desc: Analog to Digital converter
 *
 */

    ADC10CTL0 |= ENC + ADC10SC;

    while ( ADC10CTL1 & ADC10BUSY );

    int mem = ADC10MEM ;

    return mem;
}
