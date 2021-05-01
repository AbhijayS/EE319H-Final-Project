#include "inc/tm4c123gh6pm.h"

#define PLAYER_A_TURN ( (~GPIO_PORTA_DATA_R) & 1<<2 )
#define PLAYER_A_FIRE ( (~GPIO_PORTA_DATA_R) & 1<<3 )

void gamepad_init(void);

