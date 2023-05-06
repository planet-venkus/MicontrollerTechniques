/*
 * Name: Subramanyam Srinivasa Venkat
 * Matriculation No: 4967444
 * Date of submission: 27 Nov 2021
 */
#include <templateEMP.h>


//intialization of flags and variables
int Button = -1;
int OnLED = 0;
int execPauseOpr = 0;
int isPlayback_flag=0;
int isFF_flag=0;
int isRW_flag=0;

/**
 * main.c
 *
 */

void main(void)
{
        initMSP();                              //initalize MSP

        P2SEL = 0x00;                           //Function Select Register -- P2SEL
        P2SEL2 = 0x00;                          //I/O function is selected

        P2DIR = 0x7F;                           //Setting P2.0, P2.1, P2.2, P2.3, P2.4, P2.5, P2.6 direction as output and P2.7 direction as input
        P2DIR &= ~BIT7;                         //P2.7 receives output of SR1(QD(1)) as input


        P2OUT &= ~ BIT5 ;                       // Clear / reset the shift register , i.e. turn all LEDs off.
        P2OUT |= BIT5 ;                         //Stop the reset mode (/CLR == 1)
        PosClkEdge();                           //Positive clock edge for shift register operation

        while(1)
        {
            if(Button==-1 && execPauseOpr==0)
            {
                    RunSequence();              //Run Normal sequence
            }
            else
            {
                if(Button==4)
                {
                    FastForwardLEDS();          //Fastforward sequence

                }
                if(Button==3)
                {
                    RewindLEDS();               //Rewind action
                }

                ReadBtnPress();

            }

        }


}

void ParallelDataLoad()
{/***
 Input Params: None
 Returns: None
 Desc: Parallel load SR1 to identify the button pressed. SR1 is in parallel data mode load when S0 and S1 are HIGH
 ***/
        P2OUT |= BIT2 + BIT3;                   //set P2.2(SR1 - S0) and P2.3(SR1 - S1) to High for parallel load mode
        P2OUT &= ~(BIT0 + BIT1);                //SR2 P2.0(SR2 - S0) and P2.1(SR2 - S1) to low (not operating in parallel mode)
        PosClkEdge();
}

void PlayBackAfterRW()
{/***
 Input Params: None
 Return: Params
 Desc: The function is triggered when PB1 (rewind button) is released. Returns to normal playback speed (1 pass per second).
 ***/

        if(OnLED==0||OnLED==5)
        {
            P2OUT |= BIT0 + BIT5 + BIT6 ;
                  P2OUT &= ~BIT1;
                  PosClkEdge();
                  __delay_cycles(200000);       // Provides 0.2 sec delay @ 1MHz clk frequency.
                  OnLED = 1;
        }
        int i;
        int count=0;
        count = 5-OnLED;
        for(i=1; i<=count; i++)
        {

                P2OUT |= BIT0 ;
                P2OUT &= ~BIT1;

                P2OUT &= ~ BIT6 ;
                PosClkEdge();
                OnLED++;
                __delay_cycles(200000);         // Provides 0.2 sec delay @ 1MHz clk frequency.
                ReadBtnPress();
        }
}

void FastForwardLEDS()
{/***
 Input Params: None
 Returns: None
 Desc: Executes fast foward action on press of PB4
 ***/

    if (OnLED == 0 || OnLED == 5)
    {
        P2OUT |= BIT0 + BIT5 + BIT6 ;
        P2OUT &= ~BIT1;

        PosClkEdge();
        __delay_cycles(100000);                 // Provides 0.1 sec delay @ 1MHz clk frequency.
        OnLED = 1;
        ReadBtnPress();
            if(isFF_flag== 0)
            {
                if(execPauseOpr==1)
                {
                    PauseLEDS();
                }
                else
                PlayBackAfterFF();

                Button = -1;
            }
    }
    else
    {
        int count=0;
        int i;
        count = 5-OnLED;
        for(i=1; i<=count; i++)
        {
            P2OUT |= BIT0 ;
            P2OUT &= ~BIT1;

            P2OUT &= ~ BIT6 ;
            PosClkEdge();
            OnLED++;
            __delay_cycles(100000);             // Provides 0.1 sec delay @ 1MHz clk frequency.

            ReadBtnPress();
            if(isFF_flag == 0)
            {
                if(execPauseOpr == 1)
                {
                    PauseLEDS();
                }
                else
                {
                    PlayBackAfterFF();
                    Button=-1;
                    break;
                }
            }
        }
    }
}

void RewindLEDS()
{/***
 Input Params: None
 Return: None
 Desc: The LEDs D1 to D4 glow in reverse direction .i,e, D4 to D1 with 2 passes per second.
 ***/
    if(OnLED == 0 || OnLED == 5)
    {
        int i;

        OnLED=4;
        for (i=0;i<4;i++)
        {
            if (i==0)
            {
                P2OUT |= BIT0 + BIT5 + BIT6 ;
                P2OUT &= ~BIT1;
                PosClkEdge();
            }

            if(i!=0)
            {
                P2OUT |= BIT0 + BIT5;
                P2OUT &= ~BIT1;
                P2OUT &= ~BIT6;
                PosClkEdge();

            }
        }
        __delay_cycles(100000);                 // Provides 0.1 sec delay @ 1MHz clk frequency.
        ReadBtnPress();
        if(isRW_flag==0)
        {
            if(execPauseOpr==1)
            {
                PauseLEDS();
            }
            if(execPauseOpr!=1)
            PlayBackAfterRW();

            Button=-1;
        }
    }
    else if(isRW_flag != 0)
    {
        int j;
        for (j=OnLED;j>0;j--)
        {
            P2OUT |= BIT1 + BIT5;
            P2OUT &= ~BIT0;

            PosClkEdge();

            OnLED--;

            ReadBtnPress();
            __delay_cycles(100000);             // Provides 0.1 sec delay @ 1MHz clk frequency.

            if(isRW_flag==0)
            {
                if(OnLED==4)
                {
                    __delay_cycles(100000);     // Provides 0.1 sec delay @ 1MHz clk frequency.
                }
                break;
            }
        }
    }
    else if(isRW_flag == 0)
    {
        if(execPauseOpr == 1)
        {
            PauseLEDS();
        }

        if(execPauseOpr!=1)
        {
            PlayBackAfterRW();
        }
        Button = -1;
    }

}

void ReadBtnPress()
{/***
 Input Params: None
 Returns: None
 Desc: The SR1 operated in parallel data load mode had output QD at P2.7. State of PB4(D) is directly read at QD. The
       state of any other pressed button is read by using shift 'RIGHT' to 'D' and reading at QD along with polling operation
       for continuous check.
 ***/
    ParallelDataLoad();                         //SR1 is in parallel data load state

    if(P2IN & BIT7)
       Button = 4;                              //QD(P2.7) is HIGH implies PB4 is pressed
    else
    {
        int i;

        P2OUT |= BIT2;                          //SR1 is operated in Shift RIGHT mode (S0 = 1 and S1 = 0)
        P2OUT &= ~BIT3;

        Button=-1;                              //Default state when no Push button is pressed

        for (i=1;i<4;i++)
        {
            PosClkEdge();

            if((P2IN & BIT7))
              Button = i;                       //When PushButton = 1 -- PB3 is pushed, PushButton = 2 -- PB2 is pushed, PushButton = 3 -- PB1 is pushed.

        }
    }
        ButtonAction();                         //Pressed button's is to be executed
}

void PauseLEDS()
{/***
 Input Params: None
 Returns: None
 Desc: SR2 is set to hold mode. Holds previous state.
 ***/
      P2OUT &= ~BIT0 ;                          // P2.0 = 0 and P2.1 = 0. SR2 is in hold mode.
      P2OUT &= ~BIT1;

      PosClkEdge();

      if(execPauseOpr == 0)
      {
          isPlayback_flag = 0;

      }

      Button = -1;
}

void PlayBackAfterFF()
{/***
 Input Params: None
 Returns: None
 Desc: Returns to Pause/Playback state after PB4 pushing is released.
 ***/
    int i;
    int count=0;
    count = 5 - OnLED;
        if(OnLED == 0)
        {
            P2OUT |= BIT0 + BIT5 + BIT6 ;       //P2.0 = 1 and P2.1 = 0 operates SR2 in Shift Right. Bit '1' is shifted in SR2 using P2.6, /CLR = 1 (stop reset mode)
            P2OUT &= ~BIT1;

            PosClkEdge();

            __delay_cycles(200000);             // Provides 0.2 sec delay @ 1MHz clk frequency.
            OnLED = 1;
        }

        for(i=1; i<=count; i++)
        {
            P2OUT |= BIT0 ;                     //2.0 = 1 and P2.1 = 0 operates SR2 in Shift Right
            P2OUT &= ~BIT1;

            P2OUT &= ~BIT6 ;                    //P2.6 = 0. Bit '0' is shifted in SR2
            PosClkEdge();

            OnLED++;

            __delay_cycles(200000);             // Provides 0.2 sec delay @ 1MHz clk frequency.
            ReadBtnPress();
        }
}

void PlaybackAction()
{/***
 Input Params: None
 Returns: None
 Desc: Playback Action is executed.
 ***/
    if(execPauseOpr == 1)
    {
        P2OUT |= BIT0;                          //SR2((S0, P2.0) = 1, (S1, P2.1) = 0) operates in shift RIGHT mode
        P2OUT &= ~BIT1;

        int i;
        int count=0;
        execPauseOpr = 0;
        count = 5-OnLED;
        isPlayback_flag=1;

            for(i=1; i<=count; i++)
            {
                P2OUT |= BIT0 ;                 //SR2((S0, P2.0) = 1, (S1, P2.1) = 0) operates in shift RIGHT mode
                P2OUT &= ~BIT1;

                P2OUT &= ~BIT6 ;                //P2.6 = 0. Bit '0' is shifted in SR2

                PosClkEdge();                   //Clock edge for SR2 operation

                OnLED++;
                __delay_cycles(200000);         //Provides 0.5 sec delay @ 1MHz clk frequency.

                ReadBtnPress();

                if(execPauseOpr == 1)
                {
                    break;
                }
            }
    }

      Button = -1;
      //isPlayback=1;
      RunSequence();

}

void PosClkEdge()
{/***
 Input Params: None
 Returns: None
 Desc: Gives positive Edge clock for the shift registers
 ***/

    P2OUT |= BIT4 ;                             // (P2.4, CK) Clock state changes from 0 to 1,i.e, positive clock edge
    P2OUT &= ~BIT4 ;                            // (P2.4, CK) Clock state is changed back to 0
}

void ButtonAction()
{/***
 Input Params: None
 Returns: None
 Desc: Pressed button action is executed.
       Button = 1 performs Playback (PB3)
       Button = 2 performs Pause (PB2)
       Button = 3 performs Rewind (PB1)
       Button = 4 performs Fast Forward (PB4)
 ***/

    if(Button == 1 && isPlayback_flag == 0){
        PlaybackAction();
    }
    else if(Button == 1 && isPlayback_flag == 1) {
        Button = -1;
    }
    else if(Button == 2)
    {
        PauseLEDS();
        execPauseOpr = 1;
    }
    else if(Button == 3)
    {
        isRW_flag = 1;
    }
    else if(Button != 3)
    {
        isRW_flag = 0;
    }
    else if(Button == 4)
    {
        isFF_flag = 1;
    }
    else if(Button != 4)
    {
        isFF_flag = 0;
    }

}

void RunSequence()
{/***
 Input Params: None
 Returns: None
 Desc: Function that performs sequential blinking at 1 pass (4 states changes) per second
 ***/
    ReadBtnPress();

    if(execPauseOpr==0)
    {
       P2OUT |= BIT0 + BIT5 + BIT6 ;
       P2OUT &= ~BIT1;

       PosClkEdge();

       __delay_cycles(200000);                  // Provides 0.2 sec delay @ 1MHz clk frequency.

       OnLED = 1;

       int i;
        for(i=1; i<=4; i++)
        {
            ReadBtnPress();

            if(Button==-1 && execPauseOpr==0)
            {
                P2OUT |= BIT0 + BIT5 + BIT6 ;
                P2OUT &= ~BIT1;
                P2OUT &= ~BIT6 ;

                PosClkEdge();

                OnLED++;
                __delay_cycles(200000); // Provides 0.2 sec delay @ 1MHz clk frequency.
            }
            else
            {
                ReadBtnPress();
            }
        }
    }
}
