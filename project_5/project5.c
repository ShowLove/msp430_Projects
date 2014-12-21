void Init_UART(void);	//should not touch this was given to me 
void OUTA_UART(unsigned char);	//should not touch this was given to me 
unsigned char INCHAR_UART(void);	//should not touch this was given to me// 
void Init_LCD(void);   //should not touch this was given to me 
void Hyperterminal_display(int);

#include "msp430fg4618.h"   
#include "stdio.h"

// there are 11 locations needed for the softbaugh LCD
// only 7 are used for the 7 segment displays
// memory locations after the 7 segments
// are used to turn on the special characters
// such as battery status, antenna, f1-f4, etc

unsigned char *LCDSeg = (unsigned char *) &LCDM3; 	// display segment starts at address LCDM3
// which is the right most segment of the LCD
int LCD_SIZE=11;

int main(void)
{
	unsigned int i, a, b;
int array[] = {0x5f, 0x06, 0x6b, 0x2f, 0x36, 0x3d, 0x7d, 0x07, 0x7f, 0x37, 0x77, 0x7c, 0x68, 0x6e, 0x79, 0x71};							// the array contains 16 characters in order: 0-9, A, b, c, d, E, F
unsigned int user_characters[5];		// contains all user�s characters
										//typing these are being typed in the Hyperterminal : EE+FF  5 charactes 

	WDTCTL = WDTPW + WDTHOLD; 		// stop watchdog timer  //I dont completely undrstand this, but this is so the program runs 
									//indefinetly 
	Init_LCD();						// initialize the LCD
	Init_UART();					// initialize UART

	for (;;)						// infinite loop
	{
		for(i=0; i<5; i++)				// get the characters from keyboard: 2 digits of 1st number, 2
										// digits of 2nd number, 1 operation character (+, -,  or *)
		{
			a=INCHAR_UART();			//get the character from the keyboard 
			OUTA_UART(a);				//output character on hyperterminal screen 

			if (a >= 0x30 && a <= 0x39)
				a = a-0x30; 		// if the digit is 0-9, convert ASCII code to 4 bit number
			if (a >= 0x41 && a <= 0x46)
				a = a-0x37; 		// if the digit is A-F, convert ASCII code to 4 bit number

			user_characters [i] = a;		// save user characters in an array
			LCDSeg[i]=0;			// clear the LCD before displaying a new character 
		}							
		//these are the ascii hexidecimal values for =, space ... 
		OUTA_UART(0x20); 			// output a space on HyperTerminal screen
		OUTA_UART(0x3D);			// output a �=�
		OUTA_UART(0x20); 			// output a space


//-----------------------------------------------------------Operations (+, -, *) ----------------------------------------------------------
		a= user_characters [0]<<4;		// shift the 1st digit of the 1st number to the left 4 bits	"E"E+01 = ...
		a=a + user_characters [1];		// combine the 1st digit and the 2nd digit to get the 1st number
										//say we have for example 13
													//0000 0001 = 1 shift this left -> 0001 0000
													//0000 0011 = 3 add this to the previous -> 0001 0011 = 13


		b= user_characters [3]<<4;		// shift the 1st digit of the 2nd number to the right 4 bits
		b=b + user_characters [4];		// combine the 1st digit and the 2nd digit to get the 2nd number  
										// same as previous but for second two numbers AA+"FF"

		if (user_characters [2] == 0x2B)		// if user enters �+�    2B is plus in ascii 
			b = b + a;			// add 2 numbers
							// the sum�s maximum # of digits = 3 (FF + FF = 1FE)
		else if (user_characters [2] == 0x2D)	// if user enters �-�	2D  is minus in ascii 
		{
			if (a < b)
			{
				b = b - a; 		// a � b < 0
				OUTA_UART(0x2D);	// output a �-� on the hyperterminal because the number is negative 
			LCDSeg[2]=0x20;	// display �-� (segment g) on the LCD
							//max mun of digits in a subtraction is 2 so the neg will go before the number 

			//  LCDSeg[]    represents the positions on the msp430 lcd 
			//	  -	  -	  -    -
			//  |   ||  ||  ||  |
			//    -   -   -	   -
			//  |   ||  ||  ||  |
			//    -   -   -	   -
			//    3   2   1   0

			}
			else
				b = a - b;
		}
		else					// if user enters �*�
			b = b * a;			// multiply 2 numbers
							// the product�s maximum # of digits = 4 (FF * FF = FE01)

//-----------------------------------------------------------LCD Display----------------------------------------------------------
		//b is the result 
		a = b & 0x0F;				// get the lowest 4 bits  0F = 0000 1111  so if you and you get the lower 4 bits 
									// the result a was two hexadecimal numbers 
									// i want to grab the right one and display it on the right most spot of the lcd 
		user_characters[3]=a;			// save the digit  //dont want to declare another array 
														   // so i am using user_char.. to save the result in  slot 3 
LCDSeg[0] = array[a];			// display the last digit on right most LCD display

if (b > 0x0F)				// result = 2,3, or 4 digits?   // if the result b is greater than one digit
{
			a = b & 0xF0;			// get the 2nd last digit
			a = a>>4;			// shift the digit to the right 4 bits to be displayed
			user_characters[2]=a;		// save the digit // dont want to declare another array so i just use user_character
	LCDSeg[1] = array[a];		// display the digit on the 2nd right most LCD display
}
if (b > 0xFF)				// result = 3 or 4 digits?
{
			a = b & 0xF00;			// get the 3rd last digit
			a = a>>8;			// shift the digit to the right 8 bits to be displayed
			user_characters[1]=a;		// save the digit
	LCDSeg[2] = array[a];		// display the digit on the 3rd right most LCD display
}
if (b > 0xFFF)				// result = 4 digits?
{
			a = b & 0xF000;			// get the 1st digit
			a = a>>12;			// shift the digit to the right 12 bits to be displayed
			user_characters[0]=a;		// save the digit
	LCDSeg[3] = array[a];		// display the digit on the 4th right most LCD display
}

//-----------------------------------------------------------HyperTerminal Display----------------------------------------------------------

if (b > 0xFFF) 				// if the product is 4 digits, display 4 digits on HyperTerminal
// screen
		{
			a = user_characters[0];
			Hyperterminal_display(a);		// i created this function 
			a = user_characters[1];
			Hyperterminal_display(a);
			a = user_characters[2];
			Hyperterminal_display(a);
			a = user_characters[3];
			Hyperterminal_display(a);
		}
		else if (b > 0xFF) 			// if the product is 3 digits, display 3 digits on screen
		{
			a = user_characters[1];
			Hyperterminal_display(a);
			a = user_characters[2];
			Hyperterminal_display(a);
			a = user_characters[3];
			Hyperterminal_display(a);
		}
		else if (b > 0x0F)			// if the product is 2 digits, display 2 digits on screen
		{
			a = user_characters[2];
			Hyperterminal_display(a);
			a = user_characters[3];
			Hyperterminal_display(a);
		}
		else					// if the product is 1 digit, display 1 digit on screen
		{
			a = user_characters[3];
			Hyperterminal_display(a);
		}
		OUTA_UART(0x0A); 			// output new line  //new line in ascii 
OUTA_UART(0x0D); 			// output carriage return   //move to the beginning of a new line 
	}
}

void Hyperterminal_display(int i)
{		//conversts to ascii because hyperterminal can only input and output ascii 
	if (i >= 0x00 && i <= 0x09)			// if the bits are a number 0 � 9
		i += 0x30;				// +30 to obtain the hex ASCII code for the number
	else						// if the bits are a letter	A � F
		i += 0x37;				// +37 to obtain the hex ASCII code for the letter
	OUTA_UART(i);				// i did not write this communicates with the hyperterminal 
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