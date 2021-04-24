TIMER0_TBV_R		EQU 0x40030054
GPIO_PORTE_DATA_R	EQU 0x400243FC
WIDTH				EQU 320

	THUMB
	AREA    DATA, ALIGN=4
	
	AREA    |.text|, CODE, READONLY, ALIGN=2
    EXPORT PixelDisplay
PixelDisplay

    ; r0 = timer1
    ; r1 = porte
    ; r2 = count
    ; r3 = pixel, timer2

	LDR R1, =GPIO_PORTE_DATA_R
	MOV R2, #0
	
	LDR R0, =TIMER0_TBV_R
	LDR R0, [R0]

; 100p @ 10Mhz
loop
    NOP;                        1
	;--VERY UNSAFE--;
	MOV R3, #0x6; white			1
	STR R3, [R1]; display		2
	;---------------;
	ADD R2, #1;					1
	CMP R2, #320;				1
	BNE loop;					2
                              ;=8
	LDR R3, =TIMER0_TBV_R
	LDR R3, [R3]
	NOP
	
	BX LR


    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file

