
#include <stdint.h>
#define SPRITE_HEIGHT 16
#define SPRITE_WIDTH 16/4

typedef struct sprite_t {
    uint8_t x;
    uint8_t y;
    uint8_t angle;
    const uint8_t (*base_image)[SPRITE_WIDTH];
} Sprite;

Sprite create_ship_sprite(
    uint8_t start_x,
    uint8_t start_y,
    uint8_t start_angle);
