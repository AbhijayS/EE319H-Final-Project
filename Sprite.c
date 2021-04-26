#include "Sprite.h"

const uint8_t SHIP_SPRITE[SPRITE_HEIGHT][SPRITE_WIDTH] = 
{
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X57, 0XD5, 0X55, 
0X55, 0X5D, 0X75, 0X55, 
0X55, 0X5D, 0X75, 0X55, 
0X55, 0X75, 0X5D, 0X55, 
0X55, 0XD5, 0X57, 0X55, 
0X55, 0XD5, 0X57, 0X55, 
0X57, 0X55, 0X55, 0XD5, 
0X57, 0X55, 0X55, 0XD5, 
0X57, 0XFF, 0XFF, 0XD5, 
0X55, 0X75, 0X5D, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
};

Sprite create_ship_sprite(
    uint8_t start_x,
    uint8_t start_y,
    uint8_t start_angle) {
    return (Sprite) {
        .x = start_x,
        .y = start_y,
        .angle = start_angle,
        .base_image = SHIP_SPRITE
    };
}

