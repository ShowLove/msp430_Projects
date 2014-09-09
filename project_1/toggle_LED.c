

//Part 3
#include "msp430fg4618.h"

int main(void)

{

 volatile unsigned int i; // tell the compiler not to optimize the variable i

													// otherwise the compiler may change how the

													// variable is used

WDTCTL = WDTPW + WDTHOLD; // stop watchdog timer so the program runs

													// indefinitely

P2DIR |= 0x06;						// set pin 1 and 2 to output 0110

           P2OUT= 0x00;		// set pin 1 and 2 to low

           for (;;)      	// infinite loop

           {

        	   if( P1IN == 0x02 )	//if pin 1 is pushed set pin 2 to high
        		   P2OUT = 0X04;
        	   if( P1IN == 0X01 )	//if pin 0 is pushed set pin 1 to high
        		   P2OUT = 0x02;
        	   if (P1IN == 0x03)	//if both pins are pushed set all to low
        		   P2OUT = 0X00;
        	   if (P1IN == 0x00)	//if no pins are pushed set all to high
        		   P2OUT = 0x06;

           }

}


