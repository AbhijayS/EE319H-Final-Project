
#include <math.h>
#include "Sprite.h"

#define RADIANS(x) (x*3.14f/180)


const uint8_t SHIP_SPRITE[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED] = 
{
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0XD5, 0X55, 
0X55, 0X57, 0X75, 0X55, 
0X55, 0X57, 0X75, 0X55, 
0X55, 0X5D, 0X5D, 0X55, 
0X55, 0X5D, 0X5D, 0X55, 
0X55, 0X75, 0X57, 0X55, 
0X55, 0X75, 0X57, 0X55, 
0X55, 0XD5, 0X55, 0XD5, 
0X55, 0XD5, 0X55, 0XD5, 
0X57, 0X55, 0X55, 0X75, 
0X57, 0XFF, 0XFF, 0XF5, 
0X55, 0X5F, 0X7D, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
};

Sprite create_ship_sprite(
    uint16_t start_x,
    uint16_t start_y,
    int16_t start_angle) {
    return (Sprite) {
        .x = start_x,
        .y = start_y,
        .angle = start_angle,
        .base_image = SHIP_SPRITE
    };
}

void write_pixel_to_sprite_buffer(uint8_t (*buffer)[SPRITE_WIDTH_COMPRESSED], uint8_t color, uint8_t x, uint8_t y) {
    uint8_t col = x/SPRITE_COMPRESSION;
    uint8_t pix = x%SPRITE_COMPRESSION;
    buffer[y][col] &= ~(PIXEL_MASK >> (pix*2)); // clear pixel
    buffer[y][col] |= (color >> (pix*2)); // set pixel
}

uint8_t read_pixel_from_sprite_buffer(const uint8_t (*buffer)[SPRITE_WIDTH_COMPRESSED], uint8_t x, uint8_t y) {
    int pos = x/SPRITE_COMPRESSION;
    int pix = x%SPRITE_COMPRESSION;
    return PIXEL_MASK & (buffer[y][pos] << (pix*2));
}

void rotate_pixel_buffer(
    const uint8_t base[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED],
    uint8_t buffer[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED],
    int16_t degrees,
    uint8_t center_x,
    uint8_t center_y,
    uint8_t radius
    ){
    for(int i = 0; i < SPRITE_HEIGHT; i++) {
        for (int j = 0; j < SPRITE_WIDTH; j++) {
            
            uint8_t color = read_pixel_from_sprite_buffer(base, j, i);

            if (color == BLACK) continue;

            if (i==center_y && j==center_x) {
                write_pixel_to_sprite_buffer(buffer, WHITE, j, i);
            }

            float distance = hypotf(center_y-i, center_x-j);
            if ( distance <= radius ) {
                float original_angle = atan2f(center_y-i, j-center_x);
                float new_angle = original_angle + RADIANS(degrees);
                int new_x = roundf(distance*cosf(new_angle));
                int new_y = roundf(distance*sinf(new_angle));
                write_pixel_to_sprite_buffer(buffer, WHITE, center_x+new_x, center_y-new_y);
            }
        }
    }
}

void clear_sprite_buffer(uint8_t buffer[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED]) {
    for (int i = 0; i < SPRITE_HEIGHT; i++) {
        for (int j = 0; j < SPRITE_WIDTH_COMPRESSED; j++) {
            buffer[i][j] = 0x55;
        }
    }
}

