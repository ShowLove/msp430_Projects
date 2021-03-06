The purpose of this program is to provide an Assembly Language implementation to read in the state of the switches into the MSP430 and to output a conditional string that describes the state of the switches to HyperTerminal.

Figure 8: MP430FG4618 UART-Print LED State to HyperTerminal in Assembly
;****************************************************************
; Console I/O through the on board UART for MSP 430X4XXX
; experimenter board RAM at 0x1100 - 0x30ff, FLASH at 0x3100;
; - 0xfbff
;****************************************************************
	.cdecls C,LIST,"msp430fg4618.h"
; cdecls tells assembler to allow the device header file
;----------------------------------------------------------------
; Main Code
;----------------------------------------------------------------
; This is the stack and variable area of RAM and begins at
; address 0x1100 can be used for program code or constants
; .sect ".stack" ; data ram for the stack ; .sect ".const" ; data rom for initialized
; data constants
; .sect ".text" ; program rom for code
; .sect ".cinit" ; program rom for global inits
; .sect ".reset" ; MSP430 RESET Vector
; .sect ".sysmem" ; data ram for initialized
; variables. Use this .sect to
; put data in RAM
;dat .byte 0x34 ; example of defining a byte
	.bss label, 4 ; allocates 4 bytes of uninitialized memory with the
				  ; name label
	.word 0x1234 ; example of defining a 16 bit word

; This is the constant area flash begins at address 0x3100 can be
; used for program code or constants
	.sect ".const" ; initialized data rom for
; constants. Use this .sect to
; put data in ROM
;strg1 .string "This is a test" ; example of a string stored
	;.byte 0x0d,0x0a ; add a CR and a LF to the string
	;.byte 0x00 ; null terminate the string in ROM
str0 .string "SW1 = 1, SW2 = 1"
	.byte 0x0d ; add a CR and a LF
	.byte 0x00 ; null terminate the string with
str1 .string "SW1 = 0, SW2 = 1"
	.byte 0x0d ; add a CR and a LF
	.byte 0x00 ; null terminate the string with
str2 .string "SW1 = 1, SW2 = 0"
	.byte 0x0d ; add a CR and a LF
	.byte 0x00 ; null terminate the string with
str3 .string "SW1 = 0, SW2 = 0"
	.byte 0x0d ; add a CR and a LF
	.byte 0x00 ; null terminate the string with
; This is the code area flash begins at address 0x3100 can be
; used for program code or constants
	.text ; program start
	.global _START ; define entry point

;----------------------------------------------------------------
START mov.w #300h,SP ; Initialize 'x1121 stackpointer
StopWDT mov.w #WDTPW+WDTHOLD,&WDTCTL ; Stop WDT
SetupP1 bis.b #0x06,&P2DIR ; Green and Yellow LEDs as output
; go initialize the uart
	call #Init_UART
Mainloop
	mov.b #0x00, &P2OUT ; Start with Green and Yellow LEDs off
	call #PRINT_LED_STATE

PROGRAM_FINISH
	jmp PROGRAM_FINISH ; Trap code execution after Toggle LEDs
;
PRINT_LED_STATE

	cmp #0x00, &P1IN
	jz printStr0

	cmp #0x01, &P1IN
	jz printStr1

	cmp #0x02, &P1IN
	jz printStr2

	cmp #0x03, &P1IN
	jz printStr3

	jmp PRINT_LED_STATE ; run code forever

printStr0
	mov.w #str0, R6 ; get address of first character of str0 into R4
	call #PRINT_STR
	jmp PRINT_LED_STATE
printStr1
	mov.w #str1, R6 ; get address of first character of str1 into R4
	call #PRINT_STR
	jmp PRINT_LED_STATE
printStr2
	mov.w #str2, R6 ; get address of first character of str2 into R4
	call #PRINT_STR
	jmp PRINT_LED_STATE
printStr3
	mov.w #str3, R6 ; get address of first character of str3 into R4
	call #PRINT_STR
	jmp PRINT_LED_STATE

TOGGLE_LED ;This routine should run forever
	call #INCHAR_UART ;Receive input from User
	call #OUTA_UART   ;Print character back to screen

	cmp #'G', R4
	jz  toggleGreen
	cmp #'Y', R4
	jz  toggleYellow
	jmp TOGGLE_LED ;Run Loop forever

toggleGreen
	xor.b #0x04,&P2OUT ; Toggle Green LED
	jmp TOGGLE_LED
toggleYellow
	xor.b #0x02,&P2OUT ; Toggle Yellow LED
	jmp TOGGLE_LED

PRINT_STR
;----------------------------------------------------------------
; Uses the INCHAR_UART Function to print a string to the screen one character at
; a time
;----------------------------------------------------------------
printChar
	mov.b @R6+,  R4
	cmp.b #0x00, R4
	jz    exit ; If finished, exit
	call  #OUTA_UART ;Print character to Screen
	jmp   printChar
exit
	ret

OUTA_UART
;----------------------------------------------------------------
; prints to the screen the ASCII value stored in register 4 and
; uses register 5 as a temp value
;----------------------------------------------------------------
; IFG2 register (1) = 1 transmit buffer is empty,
; UCA0TXBUF 8 bit transmit buffer
; wait for the transmit buffer to be empty before sending the
; data out
	push R5
lpa mov.b &IFG2,R5
	and.b #0x02,R5
	cmp.b #0x00,R5
	jz lpa
; send the data to the transmit buffer UCA0TXBUF = A;
	mov.b R4,&UCA0TXBUF
	pop R5
	ret
INCHAR_UART
;----------------------------------------------------------------
; returns the ASCII value in register 4
;----------------------------------------------------------------
; IFG2 register (0) = 1 receive buffer is full,
; UCA0RXBUF 8 bit receive buffer
; wait for the receive buffer is full before getting the data
	push R5
lpb mov.b &IFG2,R5
	and.b #0x01,R5
	cmp.b #0x00,R5
	jz lpb
	mov.b &UCA0RXBUF,R4
	pop R5
; go get the char from the receive buffer
	ret
Init_UART
;----------------------------------------------------------------
; Initialization code to set up the uart on the experimenter board to 8 data,
; 1 stop, no parity, and 9600 baud, polling operation
;----------------------------------------------------------------

;P2SEL=0x30;
; transmit and receive to port 2 b its 4 and 5
	mov.b #0x30,&P2SEL
; Bits p2.4 transmit and p2.5 receive UCA0CTL0=0
; 8 data, no parity 1 stop, uart, async
	mov.b #0x00,&UCA0CTL0
; (7)=1 (parity), (6)=1 Even, (5)= 0 lsb first,
; (4)= 0 8 data / 1 7 data, (3) 0 1 stop 1 / 2 stop, (2-1) --
; UART mode, (0) 0 = async
; UCA0CTL1= 0x41;
	mov.b #0x41,&UCA0CTL1
; select ALK 32768 and put in software reset the UART
; (7-6) 00 UCLK, 01 ACLK (32768 hz), 10 SMCLK, 11 SMCLK
; (0) = 1 reset
;UCA0BR1=0;
; upper byte of divider clock word
	mov.b #0x00,&UCA0BR1
;UCA0BR0=3; ;
; clock divide from a clock to bit clock 32768/9600 = 3.413
	mov.b #0x03,&UCA0BR0
; UCA0BR1:UCA0BR0 two 8 bit reg to from 16 bit clock divider
; for the baud rate
;UCA0MCTL=0x06;
; low frequency mode module 3 modulation pater used for the bit
; clock
	mov.b #0x06,&UCA0MCTL
;UCA0STAT=0;
; do not loop the transmitter back to the receiver for echoing
	mov.b #0x00,&UCA0STAT
; (7) = 1 echo back trans to rec
; (6) = 1 framing, (5) = 1 overrun, (4) =1 Parity, (3) = 1 break
; (0) = 2 transmitting or receiving data
;UCA0CTL1=0x40;
; take UART out of reset
	mov.b #0x40,&UCA0CTL1
;IE2=0;
; turn transmit interrupts off
	mov.b #0x00,&IE2
; (0) = 1 receiver buffer Interrupts enabled
; (1) = 1 transmit buffer Interrupts enabled
;----------------------------------------------------------------
;****************************************************************
;----------------------------------------------------------------
; IFG2 register (0) = 1 receiver buffer is full, UCA0RXIFG
; IFG2 register (1) = 1 transmit buffer is empty, UCA0RXIFG
; UCA0RXBUF 8 bit receiver buffer, UCA0TXBUF 8 bit transmit
; buffer
	ret
;----------------------------------------------------------------
; Interrupt Vectors
;----------------------------------------------------------------
	.sect ".reset" ; MSP430 RESET Vector
	.short START ;
	.end

