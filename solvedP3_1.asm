;Write an assembly language program to add two
;16-bit words stored at locations IN1 and IN2 and
;then stores the result in the location result

		.cdecls C, LIST, "msp430g2231.h" 	;requires for launchpad
		.sect ".system"				;start at 0x200 msp430g2231
		; data variables here

in1		.word 	0x0100
in2		.word		0x0200
result	.word	0x0000

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

			mov &in1, r5
			mov &in2, r6
			add r5, r6
			mov r6, &result

ed 		jmp		ed													; this is the finish of the code

;------------------------------------------------------------------------------------

			.sect		".reset"
			.short	STRT
			.end

