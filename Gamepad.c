#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "Gamepad.h"

#define PORTA_PINS (0xF<<2)

// Port A: 2,3,4,5
void gamepad_init(void) {

    SYSCTL_RCGCGPIO_R |= 1; // clock to port a
    
    __asm__{
        NOP
        NOP
    };
    
    GPIO_PORTA_DIR_R &= ~PORTA_PINS; // inputs

    GPIO_PORTA_PUR_R |= PORTA_PINS; // pull ups

    GPIO_PORTA_DEN_R |= PORTA_PINS; // enable


}
