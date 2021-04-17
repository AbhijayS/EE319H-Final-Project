/* This file contains sprite definitions */

#ifndef __SPRITES_H__
#define __SPRITES_H__

#include <stdint.h>

#define HEIGHT 9
#define WIDTH 9
static const uint8_t BMP_SHIP[] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 1, 0, 0, 0, 0, 
0, 0, 0, 1, 1, 1, 0, 0, 0, 
0, 0, 0, 1, 0, 1, 0, 0, 0, 
0, 0, 1, 1, 1, 1, 1, 0, 0, 
0, 0, 0, 1, 0, 1, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0
};

void bufferSprite(const uint8_t *bmp, uint8_t x, uint8_t y);

#endif
