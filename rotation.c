#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define RADIANS(x) (x*3.14f/180)

#define HEIGHT 16
#define WIDTH 16
#define COMPRESSION 4

#define WHITE ((uint8_t)0xC0)
#define BLACK ((uint8_t)0x40)
#define PIXEL_MASK ((uint8_t)0xC0)


uint8_t img[HEIGHT][WIDTH/COMPRESSION] = {
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




void writePixel(uint8_t a[HEIGHT][WIDTH/COMPRESSION], uint8_t color, uint16_t x, uint16_t y) {
  uint16_t col = x/COMPRESSION;
  uint16_t pix = x%COMPRESSION;
  a[y][col] &= ~(PIXEL_MASK >> (pix*2)); // clear pixel
  a[y][col] |= (color >> (pix*2)); // set pixel
}

int readPixel(uint8_t a[HEIGHT][WIDTH/COMPRESSION], int x, int y) {
    int pos = x/COMPRESSION;
    int pix = x%COMPRESSION;
    return PIXEL_MASK & (a[y][pos] << (pix*2));
}

void p2d(uint8_t a[HEIGHT][WIDTH/COMPRESSION]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (readPixel(a, j, i)==BLACK) printf("   ");
            else printf(" 1 ");
        }
        printf("\n");
    }
}

void rotate_pixel_buffer(
    uint8_t base[HEIGHT][WIDTH/COMPRESSION],
    uint8_t buffer[HEIGHT][WIDTH/COMPRESSION],
    int16_t degrees,
    uint8_t center_x,
    uint8_t center_y,
    uint8_t radius
    ) {
    for(int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            
            uint8_t color = readPixel(base, j, i);

            if (color == BLACK) continue;

            if (i==center_y && j==center_x) {
                writePixel(buffer, WHITE, j, i);
            }

            float distance = hypotf(center_y-i, center_x-j);
            if ( distance <= radius ) {
                float original_angle = atan2f(center_y-i, j-center_x);
                float new_angle = original_angle + RADIANS(degrees);
                int new_x = roundf(distance*cosf(new_angle));
                int new_y = roundf(distance*sinf(new_angle));
                writePixel(buffer, WHITE, center_x+new_x, center_y-new_y);
            }
        }
    }
}

void clear_sprite_buffer(uint8_t buffer[HEIGHT][WIDTH/COMPRESSION]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH/COMPRESSION; j++) {
            buffer[i][j] = 0x55;
        }
    }
}

int main(void) {
    p2d(img);
    uint8_t res[HEIGHT][WIDTH/COMPRESSION];
    clear_sprite_buffer(res);
    p2d(res);
    rotate_pixel_buffer(img, res, 0, 8, 7, 8);
    p2d(res);
    // for (int i = 0; i < HEIGHT; i++) {
    //     for (int j = 0; j < WIDTH; j++) {
    //         if (img[i][j]==0) continue;

    //         if (i==CENTER_Y && j==CENTER_X) {
    //             res[i][j] = img[i][j];
    //         }

    //         else if ( hypotf(CENTER_Y-i, CENTER_X-j) <= RADIUS ) {
    //             float radius = hypotf(CENTER_Y-i, CENTER_X-j);
    //             float original_angle = atan2f(CENTER_Y-i, j-CENTER_X);
    //             float new_angle = original_angle + ANGLE;
    //             int new_x = roundf(radius*cosf(new_angle));
    //             int new_y = roundf(radius*sinf(new_angle));
    //             res[CENTER_Y-new_y][CENTER_X+new_x] = 1;
    //             // printf("%d, %d\n", i,j);
    //         }
    //     }
    // }
    // p2d(res);
}