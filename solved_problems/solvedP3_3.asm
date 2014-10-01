;Write an assembly language program to convert
;an upper case ASCII character stored at the 
;location chr1 to a lower case ASCII character
;stored at the location chr2

		.cdecls C, LIST, "msp430g2231.h" 	;requires for launchpad
		.sect ".system"				;start at 0x200 msp430g2231
		; data variables here

chr1	.char 'A'
chr2	.char ' '

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


			mov.b &chr1, r5
			add.b #0x20, r5
			mov.b r5, &chr2

ed 		jmp		ed													; this is the finish of the code

;------------------------------------------------------------------------------------

			.sect		".reset"
			.short	STRT
			.end
