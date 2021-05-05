
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "Pot.h"

// ADC initialization function 
// Input: sac sets hardware averaging
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(uint32_t sac){
	// port D clock
	SYSCTL_RCGCGPIO_R |= 8;
	
	__asm__{
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
	};

	while((SYSCTL_PRGPIO_R&0x0008) == 0){};// ready?
	
	GPIO_PORTD_DIR_R &= ~0x04;      // 2) make PD2 input
	GPIO_PORTD_AFSEL_R |= 0x04;     // 3) enable alternate fun on PE4
	GPIO_PORTD_DEN_R &= ~0x04;      // 4) disable digital I/O on PE4
	GPIO_PORTD_AMSEL_R |= 0x04;     // 5) enable analog fun on PE4
	SYSCTL_RCGCADC_R |= 0x01;       // 6) activate ADC0
	

	ADC0_PC_R = 0x01;               // 7) configure for 125K 
	ADC0_SSPRI_R = 0xFFFF;          // 8) Seq 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+5;  // 11) Ain9 (PE4)
	ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
	ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  	ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
	ADC0_SAC_R = sac;
}


//------------ADCIn------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){
	uint32_t data;
	ADC0_PSSI_R = 0x0008;            
	while((ADC0_RIS_R&0x08)==0){};   
	data = ADC0_SSFIFO3_R&0xFFF; 
	ADC0_ISC_R = 0x0008; 
	return data;
}

void pot_init(void) {

	// ADC_Init(0);

	// Timer B Module 0 is bit 20 (104) NVIC
	// enable NVIC interrupts
	NVIC_EN0_R |= 1<<20;
	NVIC_PRI5_R &= ~(0x7<<5); // clear priority
	NVIC_PRI5_R |= 0x2<<5; // set priority to 2

    SYSCTL_RCGCTIMER_R |= 1; // provide clock to GPTM0
	// wait a few cycles
	__asm__{
		NOP
		NOP
		NOP
		NOP
	};
    
	// GPTM0 Timer B - Pot Timer
	// 1. Ensure the timer is disabled (the TnEN bit is cleared) before making any changes.
	TIMER0_CTL_R &= ~(1<<8);
	// 2. Write the GPTM Configuration (GPTMCFG) register with a value of 0x0000.0000.
	TIMER0_CFG_R = 0x4; // configure 16-bit mode
	// 3. Configure the TnMR field in the GPTM Timer n Mode Register (GPTMTnMR)
	TIMER0_TBMR_R = (TIMER0_TBMR_R & ~3) | 2; // write 0x2 in bits 1:0 for periodic mode
	//TIMER0_TBMR_R |= 1<<5; // enable match interrupts
	// 4. Optionally configure the TnSNAPS, TnWOT, TnMTE, and TnCDIR bits in the GPTMTnMR register
	// to select whether to capture the value of the free-running timer at time-out, use an external
	// trigger to start counting, configure an additional trigger or interrupt, and count up or down.
	TIMER0_TBMR_R |= 1<<6; // wait for Timer A trigger
	// 5. Load the start value into the GPTM Timer n Interval Load Register (GPTMTnILR).
	TIMER0_TBILR_R = 45219; // .69ms
	TIMER0_TBPR_R = 40; // 32ms
	//TIMER0_TBMATCHR_R = 1880; // 40us display window

	// 6. If interrupts are required, set the appropriate bits in the GPTM Interrupt Mask Register (GPTMIMR).
	TIMER0_IMR_R |= 1<<8; // enable timeout interrupts
	//TIMER0_IMR_R |= 1<<11; // enable match interrupts
	
	TIMER0_CTL_R |= 1<<8; // enable timer b
	
}

int volume = 0;

void Timer0B_Handler(void) {
	__asm__{
		NOP
		NOP
		NOP
		NOP
	};
	
	// volume = ADC_In();
}

