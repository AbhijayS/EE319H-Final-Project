#include <stdint.h>
extern const uint8_t shoot_sound[4080];
extern volatile uint8_t volume;
void sound_update(void);
void sound_start(void);
void sound_stop(void);
void sound_init(void);
