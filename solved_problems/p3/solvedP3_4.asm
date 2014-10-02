;Write an assembly language program to convert two ASCII hex characters 
;("0-9" and "A-F") stored at the locations chr1 and chr2 to an eight bit 
;hex number to be stored at the location hex. Assume only valid
;hex ASCII characters and only upper case ASCII Letters are used. 

		.cdecls C, LIST, "msp430g2231.h" 	;requires for launchpad
		.sect ".system"				;start at 0x200 msp430g2231
		; data variables here

chr1	.char 'A'
chr2	.char '1'
hex		.byte 0x00

			.sect ".const"

			;data constants go here

			.sect ".stack"			;placed at the end of memory 0x27e msp430g2231
stk		.word		0x0000

;-------------------------------------------------------------------------------------

			.text														;program start
;			.global _STRT			;define entry point	
STRT	mov.w	#stk, SP				;Initialize end of ram for msp430g2231 launchpad
StopWDT mov.w	#WDTPW_WDTHOLD, &WDTCTL	; stop WDT

;------------------------------------------------------------------------------------

			mov.b &chr1, r5	;get the first char
			cmp.b #0x41, r5	;see if it is a number
			jlo num1
			sub.b #0x37, r5	;if A-F subtract 0x37
			jmp lp1
num1			sub.b #0x30, r5	;if a number subtract 0x30
lp1			rla.b r5
			rla.b r5
			rla.b r5
			rla.b r5				;shift to the upper 4 bits
			mov.b r5, &hex	;get the second char
			mov.b &chr2, r5	;see if it is a number
			jlo num2
			sub.b #0x37, r5	;if  A-F subtract 0x37
			jmp lp2
num2			sub.b #0x30, r5	;if a number subtract 0x30
lp2			bis.b r5, &hex	;or the lower 4-bits with the upper 4-bits


ed 		jmp		ed													; this is the finish of the code

;------------------------------------------------------------------------------------

			.sect		".reset"
			.short	STRT
			.end

