#include "inc/tm4c123gh6pm.h"

typedef enum button_state_t {released, pressed, releasing, pressing} button_state;
extern button_state player_a_fire_state;
extern button_state player_b_fire_state;
extern button_state player_a_turn_state;
extern button_state player_b_turn_state;

void gamepad_init(void);
void gamepad_update(void);
void gamepad_reset_state(void);

