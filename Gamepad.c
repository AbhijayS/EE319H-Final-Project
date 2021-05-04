#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "Gamepad.h"

#define PORTA_PINS (0xF<<2)
#define PLAYER_A_TURN ( (~GPIO_PORTA_DATA_R) & 1<<2 )
#define PLAYER_A_FIRE ( (~GPIO_PORTA_DATA_R) & 1<<3 )
#define PLAYER_B_TURN ( (~GPIO_PORTA_DATA_R) & 1<<4 )
#define PLAYER_B_FIRE ( (~GPIO_PORTA_DATA_R) & 1<<5 )

button_state player_a_fire_state;
button_state player_b_fire_state;
button_state player_a_turn_state;
button_state player_b_turn_state;

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

    gamepad_reset_state();

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

    if (PLAYER_A_TURN) {
        if (player_a_turn_state==released) {
            player_a_turn_state = pressing;
        } else {
            player_a_turn_state = pressed;
        }
    } else {
        if (player_a_turn_state==pressed) {
            player_a_turn_state = releasing;
        } else {
            player_a_turn_state = released;
        }
    }
    
    if (PLAYER_B_TURN) {
        if (player_b_turn_state==released) {
            player_b_turn_state = pressing;
        } else {
            player_b_turn_state = pressed;
        }
    } else {
        if (player_b_turn_state==pressed) {
            player_b_turn_state = releasing;
        } else {
            player_b_turn_state = released;
        }
    }
}

void gamepad_reset_state(void) {
    player_a_fire_state = released;
    player_b_fire_state = released;
    player_a_turn_state = released;
    player_b_turn_state = released;
}

