#include "Sprites.h"
#include "SSD1306.h"

void bufferSprite(const uint8_t *bmp, uint8_t x, uint8_t y) {
    // TODO: parameter checking

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            SSD1306_DrawPixel(
                    x+i, // x
                    y+j, // y
                    *(bmp + i + WIDTH*j) // color
                );
        }
    }
}
