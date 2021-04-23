#include <stdint.h>
#include "RCA.h"
#include "../inc/tm4c123gh6pm.h"

#define LINES 262
#define VSYNC_LINE 248
#define TIMEOUT 5080
#define LONG_PULSE 4704
#define SHORT_PULSE 376
#define PIXEL_CLOCK 10  // 80Mhz / 10 = 8 Mhz
#define GROUND() (GPIO_PORTE_DATA_R &= ~0x6) // PE21 = 00
#define BLACK_SAFE() (GPIO_PORTE_DATA_R = (GPIO_PORTE_DATA_R & ~0x6) | 0x2) // PE21 = 10 (slower but safe)
#define BLACK_UNSAFE() (GPIO_PORTE_DATA_R = 0x2) // PE21 = 10 (faster but less safe)
#define WHITE() (GPIO_PORTE_DATA_R |= 0x6) // PE21 = 11

#define ENABLE_DISPLAY_TIMER() \
	TIMER0_TBILR_R = 1200; /* 15us display window offset */ \
	TIMER0_CTL_R |= 1<<8; \

#define DISABLE_DISPLAY_TIMER() (TIMER0_CTL_R &= ~(1<<8))

// #define ENABLE_PIXEL_TIMER() (TIMER1_CTL_R |= 1)
// #define DISABLE_PIXEL_TIMER() (TIMER1_CTL_R &= ~1) /* disable timer */ \

static volatile uint16_t line = 0;
static volatile uint16_t pixel = 0;

void RCA_init(void) {
	// RCA output on PE0
	SYSCTL_RCGCGPIO_R |= 1<<4; // provide clock on port e
	// wait a few
	__asm__{
		NOP
		NOP
	};
	GPIO_PORTE_DIR_R |= 0x6; // make PE12 outputs
	GPIO_PORTE_DR8R_R |= 0x6; // select 8mA drive on PE12
	GPIO_PORTE_DEN_R |= 0x6; // digital enable on PE12

	// Timer A Module 0 is bit 19 (104) NVIC
	// enable NVIC interrupts
	NVIC_EN0_R |= 1<<19;
	NVIC_PRI4_R &= 0x1FFFFFFF; // clear priority
	NVIC_PRI4_R |= 0x20000000; // set priority to 1
	// Timer B Module 0 is bit 20 (104) NVIC
	// enable NVIC interrupts
	NVIC_EN0_R |= 1<<20;
	NVIC_PRI5_R &= ~(0x7<<5); // clear priority
	NVIC_PRI5_R |= 0x2<<5; // set priority to 2
	
	// initialize general purpose timers to produce accurate signals
	// 80Mhz system clock Periodic mode 15,750Hz freq (63.55us period) 7.4% duty cycle
	// To use a GPTM, the appropriate TIMERn bit must be set in the RCGCTIMER or RCGCWTIMER register (see page 338 and page 357).
	SYSCTL_RCGCTIMER_R |= 11; // provide clock to GPTM0 and 1
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
	TIMER0_CFG_R = 0x4; // configure 16-bit mode
	// 3. Configure the TnMR field in the GPTM Timer n Mode Register (GPTMTnMR)
	TIMER0_TAMR_R = (TIMER0_TAMR_R & ~3) | 2; // write 0x2 in bits 1:0 for periodic mode
	TIMER0_TAMR_R |= 1<<5; // enable match interrupts
	// 4. Optionally configure the TnSNAPS, TnWOT, TnMTE, and TnCDIR bits in the GPTMTnMR register
	// to select whether to capture the value of the free-running timer at time-out, use an external
	// trigger to start counting, configure an additional trigger or interrupt, and count up or down.
	// 5. Load the start value into the GPTM Timer n Interval Load Register (GPTMTnILR).
	TIMER0_TAILR_R = TIMEOUT-1; // 80e6/15750 ~ 5080
	TIMER0_TAMATCHR_R = LONG_PULSE; // match interrupt (58.8/63.5)*5080 ~ 4704
	// 6. If interrupts are required, set the appropriate bits in the GPTM Interrupt Mask Register (GPTMIMR).
	TIMER0_IMR_R |= 1; // enable timeout interrupts
	TIMER0_IMR_R |= 1<<4; // enable match interrupts



	// GPTM0 Timer B - Display Timer
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
	TIMER0_TBILR_R = 1200; // 15us display window offset
	//TIMER0_TBMATCHR_R = 1880; // 40us display window
	// 6. If interrupts are required, set the appropriate bits in the GPTM Interrupt Mask Register (GPTMIMR).
	TIMER0_IMR_R |= 1<<8; // enable timeout interrupts
	//TIMER0_IMR_R |= 1<<11; // enable match interrupts
	
	TIMER0_CTL_R |= 1; // enable timer a

}

volatile uint8_t badstuff = 0;
extern void HardFault_Handler(void);
volatile uint32_t ta_entry;
volatile uint32_t ta_exit;
volatile uint32_t ta_event;
volatile uint32_t ta_tb_val;
void Timer0A_Handler(void) {
	ta_entry = TIMER0_TAV_R;
	ta_event = TIMER0_MIS_R;
	ta_tb_val = TIMER0_TBV_R;
	
	if (badstuff) {
		HardFault_Handler();
	}
	badstuff = 1;
	uint32_t timera1 = TIMER0_TAV_R;
	// record interrupt event
	uint32_t event = TIMER0_MIS_R;
	// clear interrupt
	TIMER0_ICR_R |= event;

	// timeout event
	if (event & 1) {
		GROUND();
		line++;
		if (line == 263) {
			line = 1;
		} else if (line == 200) {
			DISABLE_DISPLAY_TIMER();
		} else if (line == VSYNC_LINE) {
		 	TIMER0_TAMATCHR_R = SHORT_PULSE;
		}
	}
	// match event
	else {
		BLACK_UNSAFE();
		if (line == VSYNC_LINE) {
		 	TIMER0_TAMATCHR_R = LONG_PULSE;
		} else if (line == 262) {
			ENABLE_DISPLAY_TIMER();			
			//uint32_t timera2 = TIMER0_TAV_R;
			//__asm__("NOP");
		}
	}
	badstuff = 0;
	ta_exit = TIMER0_TAV_R;
}


extern void PixelDisplay(void);
volatile uint32_t tb_entry;
volatile uint32_t tb_exit;
volatile uint32_t tb_event;
volatile uint32_t tb_ta_val;
// Display Timer
// one worry: match interrupt routine takes longer than expected and corrupts next timeout interrupt
// tuning params: reduce display time (match_reg), #pixels, display window offset (initial load value), first pixel start delay
void Timer0B_Handler(void) {
	tb_entry = TIMER0_TBV_R;
	tb_event = TIMER0_MIS_R;
	tb_ta_val = TIMER0_TAV_R;
	if (badstuff) {
		HardFault_Handler();
	}
	badstuff = 1;
	uint32_t eventb = TIMER0_MIS_R;

	// clear interrupt event
	TIMER0_ICR_R |= eventb;
	
	// start
	if (eventb & 0x100) {
		// reset period
		TIMER0_TBILR_R = TIMEOUT-1;
		// 10mhz code
		PixelDisplay();
		BLACK_UNSAFE();
	}
	// end
	else {
		BLACK_UNSAFE();
	}
	badstuff = 0;
	tb_exit = TIMER0_TBV_R;
}
