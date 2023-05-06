#include <msp430.h> 
#include <templateEMP.h>


/**Author: Subramanyam Srinivasa Venkat
 * Matriculation Number: 4967444
 * main.c
 */
int main(void)
{
    //Initializing the MSP430 board
    initMSP();

    //Setting P1.4 and P1.5 as output pins
	P1DIR |= 0x30;
	
	while(1){
	    if(P1OUT & BIT5){
	        serialPrint("ON"); //When Blue Led is ON
	    }
	    else{
	        serialPrint("OFF"); //When Blue Led is OFF
	    }

	    //Toggle P1.4
	    P1OUT ^= BIT4;

	    //Toggle P1.5
	    P1OUT ^= BIT5; //This pin can be used as Timer

	    __delay_cycles(500000); //500ms delay

	    //Toggle P1.4
	    P1OUT ^= BIT4;

	    __delay_cycles(500000);


	}

	return 0;
}
