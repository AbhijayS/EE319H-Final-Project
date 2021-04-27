#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define RADIANS(x) (x*3.14f/180)

#define HEIGHT 16
#define WIDTH 16
#define COMPRESSION 4
#define CENTER_X 8
#define CENTER_Y 7
#define RADIUS 8
#define ANGLE (RADIANS(-30))

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


uint8_t res[HEIGHT][WIDTH/COMPRESSION] = {
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
0X55, 0X55, 0X55, 0X55, 
};


void writePixel(uint8_t a[HEIGHT][WIDTH/COMPRESSION], uint8_t color, uint16_t x, uint16_t y) {
  uint16_t col = x/4;
  uint16_t pix = x%4;
  a[y][col] &= ~(0xC0 >> (pix*2)); // clear pixel
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

int main(void) {
    p2d(img);
    // p2d(res);

    for(int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i==CENTER_Y && j==CENTER_X) {
                res[i][j] = img[i][j];
            }

            uint8_t color = readPixel(img, j, i);

            if (color == BLACK) continue;

            else if ( hypotf(CENTER_Y-i, CENTER_X-j) <= RADIUS ) {
                float radius = hypotf(CENTER_Y-i, CENTER_X-j);
                float original_angle = atan2f(CENTER_Y-i, j-CENTER_X);
                float new_angle = original_angle + ANGLE;
                int new_x = roundf(radius*cosf(new_angle));
                int new_y = roundf(radius*sinf(new_angle));
                writePixel(res, WHITE, CENTER_X+new_x, CENTER_Y-new_y);
                // res[CENTER_Y-new_y][CENTER_X+new_x] = 1;
                // printf("%d, %d\n", i,j);
            }
        }
        // printf("\n");
    }
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