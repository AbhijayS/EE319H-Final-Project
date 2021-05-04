/*
  File: Lab10.c
  Author: Abhijay Saini
  Lab 10 Final Project
 */

/* 
 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include <math.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/CortexM.h"
#include "TExaS.h"
#include "Sprite.h"
#include "RCA.h"
#include "Images.h"
#include "Gamepad.h"

#define PB54                  (*((volatile uint32_t *)0x400050C0)) // bits 5-4
#define PF321                 (*((volatile uint32_t *)0x40025038)) // bits 3-1
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PB5       (*((volatile uint32_t *)0x40005080)) 
#define PB4       (*((volatile uint32_t *)0x40005040))

/* I know it's weird. Don't ask me why. */
#define WHITE ((uint8_t)0xC0)
#define BLACK ((uint8_t)0x40)
#define PIXEL_MASK ((uint8_t)0xC0)
#define RADIANS(x) ( (x)*0.01745329251f )
#define DIVISOR 2 // divides the frame rate for slower speed
#define SHIP_SPEED 2
#define BULLET_SPEED 3
#define SHIP_TURN_SPEED 10 // degrees

typedef struct bullet_t {
    uint16_t x;
    uint16_t y;
    const uint8_t width;
    const uint8_t height;
    uint8_t active;
    int8_t dx;
    int8_t dy;
} Bullet;

/*
each location is actually 4 pixels
(uint8) MAP[i][j] = AB CD EF GH
where AB/CD/EF/GH = 01 (black) or 11 (white) due to how its implemented in low-level

For example to write 11 (white) to CD:
1. mask = 11 00 00 00 => 0xC0
2. We clear CD
  MAP[i][j] &= ~(mask >> 2)
  - mask >> 2     => 00 11 00 00
  - ~(mask >> 2)  => 11 00 11 11
  - after anding => MAP[i][j] => AB 00 EF GH
3. We write 11 to CD
color = 11 00 00 00 (white)
MAP[i][j] |= (color >> 2)
  - color >> 2  => 00 11 00 00
  - result => MAP[i][j] => AB 11 EF GH

doesn't check for out of bounds or invalid colors
can be more efficient
*/
void write_pixel_to_map(uint8_t color, uint16_t x, uint16_t y) {
  uint16_t col = x/COMPRESSION;
  uint16_t pix = x%COMPRESSION;
  MAP[y][col] &= ~(PIXEL_MASK >> (pix*2)); // clear pixel
  MAP[y][col] |= (color >> (pix*2)); // set pixel
}

// doesn't check for out of bounds
// doesnt check for corrupt pixel color
// can be made more efficient
// DEPRECATED
void write_4_pixels_to_map(uint8_t pixels, uint16_t x, uint16_t y) {
  for (int i = 0; i < 4; i++) {
    write_pixel_to_map( (pixels << (i*2)) & PIXEL_MASK, x+i, y);
  }
}

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

// uint8_t collision_bullet(Bullet *bullet, direction dir) {
//   switch (dir)
//   {
//   case up:
//     for (int i = 0; i < bullet->width; i++) {
//       if (read_pixel_from_map(bullet->x+i, bullet->y-1)==WHITE) return 1;
//     }
//     return 0;

//   case down:
//     for (int i = 0; i < bullet->width; i++) {
//       if (read_pixel_from_map(bullet->x+i, bullet->y+bullet->height)==WHITE) return 1;
//     }
//     return 0;

//   case left:
//     for (int i = 0; i < bullet->height; i++) {
//       if (read_pixel_from_map(bullet->x-1, bullet->y+i)==WHITE) return 1;
//     }
//     return 0;
  
//   case right:
//     for (int i = 0; i < bullet->height; i++) {
//       if (read_pixel_from_map(bullet->x+bullet->width, bullet->y+i)==WHITE) return 1;
//     }
//     return 0;
    
//   default:
//     return 0;
//   }
// }

int bullet_collision(Bullet *bullet) {
  for (int i = -1; i < bullet->height+1; i++) {
    for (int j = -1; j < bullet->width+1; j++) {
      if (read_pixel_from_map(bullet->x+j, bullet->y+i)==WHITE) {
        return 1;
      }
    }
  }
  return 0;
}

int overlap(int x1, int x2, int y1, int y2, int x3, int x4, int y3, int y4) {
  if (x1 <= x4 &&
      x2 >= x3 && 
      y1 <= y4 &&
      y2 >= y3) return 1;
  return 0;
}

uint32_t start;
uint32_t stop;
uint32_t delay;



int
main(void)
{
  DisableInterrupts();
  PLL_Init();
  RCA_init();
  EnableInterrupts(); // enable all interrupts
  gamepad_init();		

  /*
1. Program the value in the STRELOAD register.
2. Clear the STCURRENT register by writing to it with any value.
3. Configure the STCTRL register for the required operation.

  */

  STRELOAD = ~0;
  STCURRENT = 0;
  STCTRL = 5;

  Sprite shipA = (Sprite) {
    .x = 75,
    .y = 160,
    .angle = 0,
    .base_image = SHIP_SPRITE
  };

  Bullet bulletA = (Bullet){
    .x = 80,
    .y = 20,
    .height = 2,
    .width = 2,
    .active = 0,
    .dx = 0,
    .dy = 0
  };

  Sprite shipB = (Sprite) {
    .x = 240,
    .y = 40,
    .angle = 180,
    .base_image = SHIPB_SPRITE
  };

  Bullet bulletB = (Bullet){
    .x = 80,
    .y = 20,
    .height = 2,
    .width = 2,
    .active = 0,
    .dx = 0,
    .dy = 0
  };
  
  uint8_t frame_count = 0; // counts the number of frames displayed

  int player_a_score = 0;
  int player_b_score = 0;

  while(1)
  {
    if (rca_busy_flag) continue; // still displaying content

    rca_busy_flag = 1;
    frame_count = (frame_count + 1) % DIVISOR;

    start = STCURRENT;

    gamepad_update();

    if (player_a_fire_state==pressing && !bulletA.active) {
      bulletA.x = shipA.x + 8 + (int)roundf(6*cosf(RADIANS(shipA.angle+90)));
      bulletA.y = shipA.y + 7 + -1*(int)roundf(6*sinf(RADIANS(shipA.angle+90)));
      bulletA.dx = (int)roundf(BULLET_SPEED*cosf(RADIANS(shipA.angle+90)));
      bulletA.dy = -1*(int)roundf(BULLET_SPEED*sinf(RADIANS(shipA.angle+90)));
      bulletA.active = 1;
    }

    if (player_b_fire_state==pressing && !bulletB.active) {
      bulletB.x = shipB.x + 8 + (int)roundf(6*cosf(RADIANS(shipB.angle+90)));
      bulletB.y = shipB.y + 7 + -1*(int)roundf(6*sinf(RADIANS(shipB.angle+90)));
      bulletB.dx = (int)roundf(BULLET_SPEED*cosf(RADIANS(shipB.angle+90)));
      bulletB.dy = -1*(int)roundf(BULLET_SPEED*sinf(RADIANS(shipB.angle+90)));
      bulletB.active = 1;
    }

    /* bulletA stuff */
    if (bulletA.active) {
      /* erase bullet from map */
      for (int i = 0; i < bulletA.height; i++) {
        for (int j = 0; j < bulletA.width; j++) {
          write_pixel_to_map(BLACK, bulletA.x+j, bulletA.y+i);
        }
      }

      // increment bullet
      bulletA.x+=bulletA.dx;
      bulletA.y+=bulletA.dy;

      // collision
      if (bullet_collision(&bulletA)) {
        if (overlap(bulletA.x-1, bulletA.x+bulletA.width,
                    bulletA.y-1, bulletA.y+bulletA.height,
                    shipB.x, shipB.x+SPRITE_WIDTH-1,
                    shipB.y, shipB.y+SPRITE_HEIGHT))
                    {
                      player_a_score++;
                    }
        bulletA.active = 0;
      }
      
      // no collision
      else {
        /* redraw bullet */
        for (int i = 0; i < bulletA.height; i++) {
          for (int j = 0; j < bulletA.width; j++) {
            write_pixel_to_map(WHITE, bulletA.x+j, bulletA.y+i);
          }
        }
      }
    }

    /* bulletB stuff */
    if (bulletB.active) {
      /* erase bullet from map */
      for (int i = 0; i < bulletB.height; i++) {
        for (int j = 0; j < bulletB.width; j++) {
          write_pixel_to_map(BLACK, bulletB.x+j, bulletB.y+i);
        }
      }

      // increment bullet
      bulletB.x+=bulletB.dx;
      bulletB.y+=bulletB.dy;

      // collision
      if (bullet_collision(&bulletB)) {
        if (overlap(bulletB.x-1, bulletB.x+bulletB.width,
                    bulletB.y-1, bulletB.y+bulletB.height,
                    shipA.x, shipA.x+SPRITE_WIDTH-1,
                    shipA.y, shipA.y+SPRITE_HEIGHT))
                    {
                      player_b_score++;
                    }
        bulletB.active = 0;
      }
      
      // no collision
      else {
        /* redraw bullet */
        for (int i = 0; i < bulletB.height; i++) {
          for (int j = 0; j < bulletB.width; j++) {
            write_pixel_to_map(WHITE, bulletB.x+j, bulletB.y+i);
          }
        }
      }
    }

    /* player A */
    if (frame_count == 0)
    {

      /* remove old ship from map */
      for (int i = 0; i < SPRITE_HEIGHT; i++) {
        for (int j = 0; j < SPRITE_WIDTH; j++) {
          write_pixel_to_map(BLACK, shipA.x + j, shipA.y + i);
        }
      }

      if (PLAYER_A_TURN) {
        shipA.angle = (shipA.angle - SHIP_TURN_SPEED) % 360;
      }

      int mx = (int)roundf(SHIP_SPEED*cosf(RADIANS(shipA.angle+90)));
      int my = -1*(int)roundf(SHIP_SPEED*sinf(RADIANS(shipA.angle+90)));

      if (mx > 0 && collision(&shipA, right, SHIP_SPEED)) mx = 0;
      else if (mx < 0 && collision(&shipA, left, SHIP_SPEED)) mx = 0;

      if (my > 0 && collision(&shipA, down, SHIP_SPEED)) my = 0;
      else if (my < 0 && collision(&shipA, up, SHIP_SPEED)) my = 0;

      shipA.x += mx;
      shipA.y += my;
      
      uint8_t sprite_buf[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED];
      clear_sprite_buffer(sprite_buf);
      rotate_pixel_buffer(shipA.base_image, sprite_buf, shipA.angle, 8, 7, 7);

      /* display ship on map */
      for (int i = 0; i < SPRITE_HEIGHT; i++) {
        for (int j = 0; j < SPRITE_WIDTH_COMPRESSED; j++) {
          write_4_pixels_to_map(sprite_buf[i][j], shipA.x + (j*4), shipA.y + i);
        }
      }
    }

    /* player B */
    else if (frame_count == 1)
    {
      /* remove old ship from map */
      for (int i = 0; i < SPRITE_HEIGHT; i++) {
        for (int j = 0; j < SPRITE_WIDTH; j++) {
          write_pixel_to_map(BLACK, shipB.x + j, shipB.y + i);
        }
      }

      if (PLAYER_B_TURN) {
        shipB.angle = (shipB.angle - SHIP_TURN_SPEED) % 360;
      }

      int mx = (int)roundf(SHIP_SPEED*cosf(RADIANS(shipB.angle+90)));
      int my = -1*(int)roundf(SHIP_SPEED*sinf(RADIANS(shipB.angle+90)));

      if (mx > 0 && collision(&shipB, right, SHIP_SPEED)) mx = 0;
      else if (mx < 0 && collision(&shipB, left, SHIP_SPEED)) mx = 0;

      if (my > 0 && collision(&shipB, down, SHIP_SPEED)) my = 0;
      else if (my < 0 && collision(&shipB, up, SHIP_SPEED)) my = 0;

      shipB.x += mx;
      shipB.y += my;
      
      uint8_t sprite_buf[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED];
      clear_sprite_buffer(sprite_buf);
      rotate_pixel_buffer(shipB.base_image, sprite_buf, shipB.angle, 8, 7, 7);

      /* display ship on map */
      for (int i = 0; i < SPRITE_HEIGHT; i++) {
        for (int j = 0; j < SPRITE_WIDTH_COMPRESSED; j++) {
          write_4_pixels_to_map(sprite_buf[i][j], shipB.x + (j*4), shipB.y + i);
        }
      }
    }

    stop = STCURRENT;
    delay = start-stop;
  }
}
