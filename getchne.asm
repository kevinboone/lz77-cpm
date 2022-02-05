;------------------------------------------------------------------------
; getchne.asm
; Implementation of a function that will read a character from the
;   console and return immediately without echoing. Note that this
;   function will upset Z80/8080 emulators, because it implements an
;   endless loop. It's fine on real hardware.
; Copyright (c)2022 Kevin Boone, GPL v3.0
;------------------------------------------------------------------------

	PUBLIC getchne_ 

bdos 		equ 5	; define BDOS entry point
bdos_rawio 	equ 6	; raw input 

;------------------------------------------------------------------------
;  getchne
;  Read a character, unbuffered, from the console, without echoing it 
;------------------------------------------------------------------------
getchne:
	PUSH    B
	PUSH    D
	PUSH    H
ne_next:
	MVI	E, 0ffh
	MVI     C, bdos_rawio
	CALL    bdos
	CPI     0
	JZ 	ne_next
	POP     H
	POP     D
	POP     B
	RET

;------------------------------------------------------------------------
;  getchne_ 
;  C wrapper for getchne 
;------------------------------------------------------------------------
getchne_:
	CALL	getchne
	MVI	H, 0 
	MOV 	L, A 
	RET

