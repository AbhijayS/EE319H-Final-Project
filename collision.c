
#include <stdio.h>
#include "Sprite.h"
#include "Images.h"

uint8_t read_pixel_from_map(uint16_t x, uint16_t y) {
    int pos = x/COMPRESSION;
    int pix = x%COMPRESSION;
    return PIXEL_MASK & (MAP[y][pos] << (pix*2));
}

// doesn't check for out of bounds speed or anything
// make speed = 1 to be safe for now
uint8_t collision(Sprite *sprite, direction dir, uint8_t speed) {
  if (dir == up) {
    for (int i = 0; i < SPRITE_WIDTH; i++) {
      for (int j = 1; j <= speed; j++) {
        if (read_pixel_from_map(i+sprite->x, sprite->y-j) == WHITE) {
          return 1;
        }
      }
    }
  } else if (dir == down) {
    for (int i = 0; i < SPRITE_WIDTH; i++) {
      for (int j = 1; j <= speed; j++) {
        if (read_pixel_from_map(i+sprite->x, sprite->y+SPRITE_HEIGHT-1+j) == WHITE) {
          return 1;
        }
      }
    }
  } else if (dir == left) {
    for (int i = 0; i < SPRITE_HEIGHT; i++) {
      for (int j = 1; j <= speed; j++) {
        if (read_pixel_from_map(sprite->x-j, i+sprite->y) == WHITE) {
          return 1;
        }
      }
    }
  } else {
    for (int i = 0; i < SPRITE_HEIGHT; i++) {
      for (int j = 1; j <= speed; j++) {
        if (read_pixel_from_map(sprite->x+SPRITE_WIDTH-1+j, i+sprite->y) == WHITE) {
          return 1;
        }
      }
    }
  }
  return 0;
}

void printMap(void) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (read_pixel_from_map(j, i)==BLACK) printf("");
            else printf("1");
        }
        printf("\n");
    }
}

void printSprite(uint8_t a[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED]) {
    for (int i = 0; i < SPRITE_HEIGHT; i++) {
        for (int j = 0; j < SPRITE_WIDTH; j++) {
            if (read_pixel_from_sprite_buffer(a, j, i)==BLACK) printf("   ");
            else printf(" 1 ");
        }
        printf("\n");
    }
}

int main(void) {
    Sprite s = create_ship_sprite(50,50,0);
    printSprite(s.base_image);
    printf("%d\n", collision(&s, right, 1));
    // printMap();
}