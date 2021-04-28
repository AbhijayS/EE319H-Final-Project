#include <stdint.h>

#define SPRITE_HEIGHT 16
#define SPRITE_WIDTH 16
#define SPRITE_COMPRESSION 4
#define SPRITE_HEIGHT_COMPRESSED SPRITE_HEIGHT
#define SPRITE_WIDTH_COMPRESSED (SPRITE_WIDTH / SPRITE_COMPRESSION)
#define WHITE ((uint8_t)0xC0)
#define BLACK ((uint8_t)0x40)
#define PIXEL_MASK ((uint8_t)0xC0)

typedef enum direction {up, down, left, right} direction;

typedef struct sprite_t {
    uint16_t x;
    uint16_t y;
    uint16_t angle;
    const uint8_t (*base_image)[SPRITE_WIDTH_COMPRESSED];
} Sprite;


Sprite create_ship_sprite(
    uint16_t start_x,
    uint16_t start_y,
    int16_t start_angle);

void write_pixel_to_sprite_buffer(uint8_t (*buffer)[SPRITE_WIDTH_COMPRESSED], uint8_t color, uint8_t x, uint8_t y);

uint8_t read_pixel_from_sprite_buffer(const uint8_t (*buffer)[SPRITE_WIDTH_COMPRESSED], uint8_t x, uint8_t y);

void rotate_pixel_buffer(
    const uint8_t base[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED],
    uint8_t buffer[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED],
    int16_t degrees,
    uint8_t center_x,
    uint8_t center_y,
    uint8_t radius
    );

void clear_sprite_buffer(uint8_t buffer[SPRITE_HEIGHT][SPRITE_WIDTH_COMPRESSED]);
