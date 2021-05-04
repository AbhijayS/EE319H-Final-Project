#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "Gamepad.h"

#define PORTA_PINS (0xF<<2)

button_state player_a_fire_state = released;
button_state player_b_fire_state = released;

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

// only updates player a fire
void gamepad_update(void) {
    if (PLAYER_A_FIRE) {
        if (player_a_fire_state==released) {
            player_a_fire_state = pressing;
        } else {
            player_a_fire_state = pressed;
        }
    } else {
        if (player_a_fire_state==pressed) {
            player_a_fire_state = releasing;
        } else {
            player_a_fire_state = released;
        }
    }
    
    if (PLAYER_B_FIRE) {
        if (player_b_fire_state==released) {
            player_b_fire_state = pressing;
        } else {
            player_b_fire_state = pressed;
        }
    } else {
        if (player_b_fire_state==pressed) {
            player_b_fire_state = releasing;
        } else {
            player_b_fire_state = released;
        }
    }
}

