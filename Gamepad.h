#include "inc/tm4c123gh6pm.h"

#define PLAYER_A_TURN ( (~GPIO_PORTA_DATA_R) & 1<<2 )
#define PLAYER_A_FIRE ( (~GPIO_PORTA_DATA_R) & 1<<3 )
#define PLAYER_B_TURN ( (~GPIO_PORTA_DATA_R) & 1<<4 )
#define PLAYER_B_FIRE ( (~GPIO_PORTA_DATA_R) & 1<<5 )

typedef enum button_state_t {released, pressed, releasing, pressing} button_state;
extern button_state player_a_fire_state;
extern button_state player_b_fire_state;

void gamepad_init(void);
void gamepad_update(void);
