//This code creates a clock on the LCD Display of the MSP430

void Init_UART(void);	//should not touch this was given to me
void OUTA_UART(unsigned char);	//should not touch this was given to me
unsigned char INCHAR_UART(void);	//should not touch this was given to me//
void Init_LCD(void);   //should not touch this was given to me
void Hyperterminal_display(int);
__interrupt void Time_ISR(void);

#include "msp430fg4618.h"
#include "intrinsics.h"
#include "stdio.h"


/////////////////////////////////////////////////////////////////////////////INTERUPT STUFF//////////////////////////////////////////////////////////////////////////

// there are 11 locations needed for the softbaugh LCD
// only 7 are used for the 7 segment displays
// memory locations after the 7 segments
// are used to turn on the special characters
// such as battery status, antenna, f1-f4, etc

unsigned char *LCDSeg = (unsigned char *) &LCDM3; 	// display segment starts at address LCDM3
// which is the right most segment of the LCD
int LCD_SIZE=11;




//****************************************************************************
// LED BLINK program using the timer A function and Interrupts
// experimenter board RAM at 0x1100  0x30ff, FLASH at 0x3100  0xfbff
// Port 2 is used for the LED's Port 2 bit 2 is the green LED,
// Port 2 bit 1 is the yellow LED
//
//****************************************************************************
// must include the C header to get the predefined variable names
//

#include "msp430fg4618.h"
#include "intrinsics.h"
// include if input and output routines are used in c
#include "stdio.h"
__interrupt void Time_ISR(void);

	unsigned int mlSec = 0;
	unsigned int mbSec = 5;
	unsigned int mlMin = 9;
	unsigned int mbMin = 5;
	unsigned int mlHour = 2;
	unsigned int mbHour = 1;


int main(void)
{
	Init_LCD();						// initialize the LCD
	Init_UART();						// initialize UART

	unsigned int i, a, b;

	int array[] = {0x5f, 0x06, 0x6b, 0x2f, 0x36, 0x3d, 0x7d, 0x07, 0x7f, 0x37, 0x77, 0x7c, 0x68, 0x6e, 0x79, 0x71};							// the array contains 16 characters in order: 0-9, A, b, c, d, E, F
	unsigned int user_characters[5];		// contains all users characters
											//typing these are being typed in the Hyperterminal : EE+FF  5 charactes
	// tell the compiler not to optimize the variable i volatile unsigned int i;
	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
	P2DIR |= 0x02; // port 2 bit 1 to as an output
	// 1=output 0 = input
	P2OUT = 0x02; // go turn the LED ON
	// TACCTL0 capture compare register
	// need only to enable interrupts with this reg all other values are left 0
	// using compare mode
	// Bit 4 is the compare/capture enable interrupt bit 1 = enabled
	// TACCTL0 = CCIE;
	TACCTL0 = 0x010;
	// next the general purpose maskable interrupt bit GEI must be set in the SR
	// register
	// to turn on the maskable interrupts
	// next must use an intrinsic function given in intrinsics.h
	// to turn interrupt on use __enable_interrupt();
	// to turn interrupt off use __disable_interrupt();
	// TACTL =TA0CTL // timer a control register
	// bits 8 and 9 set the clock input 00=TACLk, 01=ACLK(32768 Hz), 10=SMCLK(1
	// MHz)
	// bits 7 and 6 set the divider 00=/1, 01=/2, 10=/4, 11=/8
	// bits 4 and 5 set the mode 00 stop, 01 up count from 0 to the value in the
	// TACCR0 reg
	// 10 counts from 0 to 0xffff, 1 up / down mode 0 to TACCR0 then to 0
	// bit 3 unused bit 2 =1 clears counter, bit 1=1 enables interrupt
	// bit 0 status Timer a interrupt flag = 1 interrupt pending
	// ACLK 01, CLR timer = 1, /8, and count up mode
	// in binary 0000 0001 1101 0100
	// TA0CTL = MC_1 | ID_3 | TASSEL_1 | TACLR;
	TA0CTL = 0x01D4;
	// TACCR0 counter value register
	// ALCK = 32768 Hz
	// for one second time t= (N*8) /32768 N=4096 = 0x1000 max count =6 5535
	TACCR0=0x1000;
	// Go enable the interrupts now that the timer is setup as desired
	__enable_interrupt();
	for (;;){
	// add main body of code here as required

		/*
				unsigned int mlSec = 0;
				unsigned int mbSec = 0;
				unsigned int mlMin = 0;
				unsigned int mbMin = 0;
				unsigned int mlHour = 0;
				unsigned int mbHour = 0;
		*/


		LCDSeg[1] = array[mlSec];
		LCDSeg[2] = array[mbSec] | 0x80;	//The first semicolon
		LCDSeg[3] = array[mlMin];
		LCDSeg[4] = array[mbMin] | 0x80;	//The Second semicolon
		LCDSeg[5] = array[mlHour];
		LCDSeg[6] = array[mbHour];

	}
}
// must now setup the interrupt function in C need to point the function
// address to the
// timer interrupt vector located at 0xFFF2, this is done with the #pragma
// statement
#pragma vector = TIMERA0_VECTOR;
// the __interrupt tells C that the following function is an interrupt
// function
// the interrupt function must pull off the stack the SR reg
__interrupt void Timer_ISR(void)
{
	//int array[] = {0x5f, 0x06, 0x6b, 0x2f, 0x36, 0x3d, 0x7d, 0x07, 0x7f, 0x37, 0x77, 0x7c, 0x68, 0x6e, 0x79, 0x71};

	// Simply blink the yellow LED using the exor operator
	P2OUT ^=0x02;
/*
		unsigned int mlSec = 0;
		unsigned int mbSec = 0;
		unsigned int mlMin = 0;
		unsigned int mbMin = 0;
		unsigned int mlHour = 0;
		unsigned int mbHour = 0;
*/
	mlSec++;

	if( mlSec == 10)
	{
		mbSec++;
		mlSec = 0;
	}
	if( mbSec == 6 )
	{
		mbSec = 0;
		mlSec = 0;
		mlMin++;
	}
	if( mlMin == 10 )
	{
		mbMin++;
		mlMin = 0;
	}
	if( mbMin == 6 )
	{
		mlMin = 0;
		mbMin = 0;
		mlHour++;
	}
	if( mlHour == 10 && mbHour == 0 )
	{
		mlHour = 0;
		mbHour = 1;
	}
	if( mlHour == 3 && mbHour == 1 )
	{
		mlHour = 1;
		mbHour = 0;
	}

}





//------------------------------------------------- UART Functions-----------------------------------------------------
void OUTA_UART(unsigned char A)
{
	// IFG2 register (1) = 1 transmit buffer is empty,
	// UCA0TXBUF 8 bit transmit buffer
	// wait for the transmit buffer to be empty before sending the data out
	do
	{}while ((IFG2&0x02)==0);

	UCA0TXBUF =A; 		// send the data to the transmit buffer
}

unsigned char INCHAR_UART(void)
{
	// IFG2 register (0) = 1 receive buffer is full,
	// UCA0RXBUF 8 bit receive buffer
	// wait for the receive buffer to be full before getting the data
	do
	{}while ((IFG2&0x01)==0);

	return (UCA0RXBUF);		// go get the char from the receive buffer
}

void Init_UART(void)
{
	// Initialization code to set up the UART on the experimenter board to 8 data,
// 1 stop, no parity
	// and 9600 baud, polling operation

	P2SEL=0x30; 			// transmit and receive to port 2 bits 4 and 5
					// bits p2.4 transmit and p2.5 receive

	UCA0CTL0=0; 			// 8 data, no parity, 1 stop, uart, async
					// (7)=1 parity, (6)=1 even, (5)= 0 lsb first,
					// (4)= 0 eight data, 1 seven data
					// (3) 0 one stop, 1 two stops
					// (2-1) -- UART mode, (0) 0 = async

	UCA0CTL1= 0x41;		// select ALK 32768 and put in software, reset the UART
					// (7-6) 00 UCLK, 01 ACLK (32768 hz)
					// 10 SMCLK, 11 SMCLK
					// (0) = 1 reset

	UCA0BR1=0; 			// upper byte of divider clock word
	UCA0BR0=3; 			// clock divide from a clock to bit clock
					// 32768/9600= 3.413
					// UCA0BR1:UCA0BR0 two 8 bit registers to form 16 bit
					// clock divider for the baud rate
	UCA0MCTL=0x06;		// low frequency mode module 3 modulation pater
					// used for the bit clock

UCA0STAT=0;                                 // do not loop the transmitter back to the receiver for echoing
					// (7) = 1 echo back transmitter to receiver
					// (6) = 1 framing, (5) = 1 overrun, (4) =1 parity
					// (3) = 1 break, (0) = 2 transmitting or receiving data

	UCA0CTL1=0x40;		// take UART out of reset
	IE2=0; 				// turn transmit interrupts off

					// IFG2 register (0) = 1 receiver buffer is full, UCA0RXIFG
					// IFG2 register (1) = 1 transmit buffer is empty, UCA0RXIFG
					// UCA0RXBUF 8 bit receiver buffer
					// UCA0TXBUF 8 bit transmit buffer
}

//------------------------------------------------- Initialize the LCD system-----------------------------------------------------
void Init_LCD(void)
{
// using the LCD A controller for the MSP430fg4618
// the pins of the LCD are memory mapped
// onto the mp430F4xxx memory bus and
// are accessed via LCDSeg[i] array

// LCD_SIZE-4 only gives the 7 segment displays
// plus DP and colons (colons / dp)
// colons are the same bit setting
// right most display is at LCDSeg[0];
// Display format
// 		AAA
// 	            F          B
// 	X          F          B
// 		GGG
//           X          E         C
//                       E         C
//           DP 	DDD
// bit order:  dp, E, G, F, D, C, B, A or  :, E, G, F, D, C, B, A
int n;
for (n=0;n<LCD_SIZE;n++)
{
// initialize the segment memory to 0 to turn off the LCD
// including all of the special characters
*(LCDSeg+n) = 0;		// can also be written as: LCDSeg[n]=0;
}
// ports 5.2-5.4 are connected to com1, com2, com3 of LCD
// and com0 is fixed and already assigned
P5SEL = 0x1C;				 // assign com1 - com3 to port5
// BIT4 | BIT3 |BIT2 = 1, P5.4, P5.3, P5.2 = 1

LCDAVCTL0 = 0x00;			// use the internal voltage for the LCD, bit 4 = 0 (VLCDEXT=0)
// internal bias voltage set to 1/3 of Vcc, charge pump disabled

LCDAPCTL0 = 0x7E;			// LCDS28-LCDS0 pins: LCDS0 = lsb and LCDS28 = MSB
// the LCD uses S4-S24, S0-S3 are not used here
// on the LCD, 28-31 not needed
// LCDACTL1 is not required since we are not using S32 - S39

LCDACTL = 0x7d;			// bit pattern required = 0111 1101 = 0x7d
// the LCD uses the ACLK as the master clock
// to scan rate for the display segments
// the ACLK has been set to 32768 Hz
// with the external 327768 Hz crystal
// let's use scan frequency of 256 Hz or a divisor of 128
// this is fast enough not to see the display flicker
// LCDFREQ division(3 bits), LCDMUX (2 bits), LCDSON segments on,
// not used, LCDON LCD module on
// 011 = freq /128,
// 11 =>  4 mux is needed since the display uses
// common inputs com0-com3
// need to turn the LCD on, LCDON = 1
// LCDSON allows the segments to be blanked good for blinking
// to display the segments, LCDSON = 1
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//****************************************************************************
// LED BLINK program using the timer A function and Interrupts
// experimenter board RAM at 0x1100  0x30ff, FLASH at 0x3100  0xfbff
// Port 2 is used for the LED's Port 2 bit 2 is the green LED,
// Port 2 bit 1 is the yellow LED
//
//****************************************************************************
// must include the C header to get the predefined variable names
//


