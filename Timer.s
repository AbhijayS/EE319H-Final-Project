TIMER0_TBV_R		EQU 0x40030054
GPIO_PORTE_DATA_R	EQU 0x400243FC
WIDTH				EQU 100
WHITE EQU 0x6

	THUMB
	AREA    DATA, ALIGN=4
	
	AREA    |.text|, CODE, READONLY, ALIGN=2

; Input R0 contains the data row
; Output display WIDTH pixels
; Return void
    EXPORT PixelDisplay
PixelDisplay

    ; r0 = timer1
    ; r1 = porte
    ; r2 = count
    ; r3 = pixel, timer2

	LDR R1, =GPIO_PORTE_DATA_R
	MOV R2, #0

; 100p @ 10Mhz
loop
	;--VERY UNSAFE--;
	LDRB R3, [R0, R2];			2
	STR R3, [R1]; display		2
	;---------------;
	ADD R2, #1;					1
	CMP R2, #100;				1
	BNE loop;					2
                              ;=8

	NOP
	
	BX LR
	

PixelDisplay2
	PUSH {R4, LR}
	LDR R1, =GPIO_PORTE_DATA_R
	MOV R2, #0
	
; 320px 10Mhz, 9Mhz
; 300px 8Mhz
; 32 for 4 pixels
loop2
	LDRB R3, [R0, R2];			2
	LSR R4, R3, #5;				1
	STR R4, [R1]; display		2
	NOP;						1
	NOP;						1
	NOP;						1

	
	LSR R4, R3, #3;				1
	NOP;						1
	NOP;						1
	STR R4, [R1]; display		2
	NOP;						1
	NOP;						1
	NOP;						1
	
	
	LSR R4, R3, #1;				1
	NOP;						1
	NOP;						1
	STR R4, [R1]; display		2
	NOP;						1
	NOP;						1
	NOP;						1	

	
	LSL R4, R3, #1;				1
	ADD R2, #1;					1
	CMP R2, #80;				1
	STR R4, [R1]; display		2
	NOP;						1
	BNE loop2;					2
                              ;=32
	
	POP {R4, LR}
	BX LR


    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file

