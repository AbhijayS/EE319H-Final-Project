#include <stdint.h>
#include "RCA.h"
#include "../inc/tm4c123gh6pm.h"

#define LINES 262
#define VSYNC_LINE 248
#define TIMEOUT 5080
#define LONG_PULSE 4704
#define SHORT_PULSE 376

static volatile uint16_t line;

void RCA_init(void) {
	line = 0;

	// RCA output on PE0
	SYSCTL_RCGCGPIO_R |= 1<<4; // provide clock on port e
	// wait a few
	__asm__{
		NOP
		NOP
	};
	GPIO_PORTE_DIR_R |= 1; // make PE0 output
	GPIO_PORTE_DR8R_R |= 1; // select 8mA drive on PE0
	GPIO_PORTE_DEN_R |= 1; // digital enable on PE0

	
	// Timer A Module 0 is bit 19 (104) NVIC
	// enable NVIC interrupts
	NVIC_EN0_R |= 1<<19;
	NVIC_PRI4_R &= 0x1FFFFFFF; // clear priority
	NVIC_PRI4_R |= 0x20000000; // set priority to 1
	
	// initialize general purpose timers to produce accurate signals
	// 80Mhz system clock Periodic mode 15,750Hz freq (63.55us period) 7.4% negative duty cycle
	// To use a GPTM, the appropriate TIMERn bit must be set in the RCGCTIMER or RCGCWTIMER register (see page 338 and page 357).
	SYSCTL_RCGCTIMER_R |= 1; // provide clock to GPTM0
	// wait a few cycles
	__asm__{
		NOP
		NOP
		NOP
		NOP
	};

	// GPTM0 Timer A - Horizontal low pulse
	// 1. Ensure the timer is disabled (the TnEN bit is cleared) before making any changes.
	TIMER0_CTL_R &= ~1;
	// 2. Write the GPTM Configuration (GPTMCFG) register with a value of 0x0000.0000.
	TIMER0_CFG_R = 0x0; // configure 16-bit mode
	// 3. Configure the TnMR field in the GPTM Timer n Mode Register (GPTMTnMR)
	TIMER0_TAMR_R = (TIMER0_TAMR_R & ~3) | 2; // write 0x2 in bits 1:0 for periodic mode
	TIMER0_TAMR_R |= 1<<5; // enable match interrupts
	// 4. Optionally configure the TnSNAPS, TnWOT, TnMTE, and TnCDIR bits in the GPTMTnMR register
	// to select whether to capture the value of the free-running timer at time-out, use an external
	// trigger to start counting, configure an additional trigger or interrupt, and count up or down.
	// 5. Load the start value into the GPTM Timer n Interval Load Register (GPTMTnILR).
	TIMER0_TAILR_R = TIMEOUT; // 80e6/15750 ~ 5080
	TIMER0_TAMATCHR_R = LONG_PULSE; // match interrupt (58.8/63.5)*5080 ~ 4704
	// 6. If interrupts are required, set the appropriate bits in the GPTM Interrupt Mask Register (GPTMIMR).
	TIMER0_IMR_R |= 1; // enable timeout interrupts
	TIMER0_IMR_R |= 1<<4; // enable match interrupts
	// 7. Set the TnEN bit in the GPTMCTL register to enable the timer and start counting.
	TIMER0_CTL_R |= 1;
	
}

void Timer0A_Handler(void) {

	// toggle PE0
	GPIO_PORTE_DATA_R ^= 1;

	// record interrupt event
	uint32_t event = TIMER0_MIS_R;
	uint16_t value = TIMER0_TAV_R;

	// clear all interrupts
	TIMER0_ICR_R |= event;

	// timeout event
	if (event & 1) {
		++line;
		if (line == VSYNC_LINE) {
			TIMER0_TAMATCHR_R = SHORT_PULSE;
		}
	}

	// match event
	else {
		if (line == VSYNC_LINE) {
			// reset match value
			TIMER0_TAMATCHR_R = LONG_PULSE;
			line = 1;
		}
	}
}
