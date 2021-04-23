TIMER1_TAV_R		EQU 0x40031050
TIMER1_ICR_R		EQU 0x40031024
GPIO_PORTE_DATA_R	EQU 0x400243FC

	THUMB
	AREA    DATA, ALIGN=4
	
	AREA    |.text|, CODE, READONLY, ALIGN=2
	EXPORT  Timer1A_Handler
Timer1A_Handler
    ; debugging ldr delay ;
    LDR R0, =TIMER1_TAV_R
    LDR R1, [R0]
    LDR R2, =TIMER1_TAV_R
    LDR R3, [R1]

	; clear interrupt
	LDR R1, =TIMER1_ICR_R
	MOV R2, #1
	STR R2, [R1]
	;

    ; r0 = timer1
    ; r1 = porte
    ; r2 = count
    ; r3 = pixel, timer2

	LDR R1, =GPIO_PORTE_DATA_R
	MOV R2, #0

    ; record interrupt
    LDR R0, =TIMER1_TAV_R
	LDR R0, [R0]

; 10Mhz
loop
    NOP                         1
	;--VERY UNSAFE--;
	MOV R3, #0x6; white			1
	STR R3, [R1]; display		2
	;---------------;
	ADD R2, #1;					1
	CMP R2, #100;				1
	BNE loop;					2
                               =8
    
    ; record interrupt
    LDR R3, =TIMER1_TAV_R
	LDR R3, [R3]

    ;--VERY UNSAFE--;
	MOV R3, #0x6; black
	STR R3, [R1]; display
	;---------------;

	BX LR
	

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file

