#include <msp430.h>

/*
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;               // Stop WDT
	P1DIR |= 0x06;                          // P1.2 and P1.3 output
	P1SEL |= 0x06;                          // P1.2 and P1.3 TA1/2 otions

	CCR0 = 23260-1;                         // PWM Period/2
	CCTL1 = OUTMOD_7;                       // CCR1 reset/set

	CCR1= 1600;                             //clockwise rotation: set ccr1 < 1677 ... counterclockwise rotation: set ccr1 > 1684
											// the further away from these numbers the value, the faster they will spin.
	TACTL = TASSEL_2+ MC_1;                	// SMCLK, up-down mode

}