#include "snake.h"
#include <string.h>

// Example 8x8 tile for the snake's head.
// This is just a simple pattern; feel free to replace with your own tile data.
// In 2bpp Game Boy format, each 8x8 tile = 16 bytes.
static const unsigned char snake_head_tile[] = {
    0xFF,0x81,0xBD,0xA5,  // First 4 bytes for rows 0-1
    0xA5,0xBD,0x81,0xFF,  // Next 4 bytes for rows 2-3
    0xFF,0xC3,0x99,0xBD,  // Next 4 bytes for rows 4-5
    0xBD,0x99,0xC3,0xFF   // Last 4 bytes for rows 6-7
};

// Simple snake representation
#define MAX_SNAKE_LENGTH 20

static UINT8 snake_x[MAX_SNAKE_LENGTH];
static UINT8 snake_y[MAX_SNAKE_LENGTH];
static UINT8 snake_length;
static Direction current_dir;

void init_snake(void) {
    // Start snake in middle of screen
    // (Game Boy screen = 160x144, but the coordinate system in GBDK uses 8x8 tiles for BG)
    // Sprites typically also use 8x8 or 8x16, so adjust to your needs.

    // 1) Load the sprite tile data into VRAM
    //    We only have 1 tile in 'snake_head_tile', so count=1
    set_sprite_data(0, 1, snake_head_tile);

    // 2) Assign that tile (#0) to sprite #0
    set_sprite_tile(0, 0);

    // 3) Initialize snake position
    snake_length = 3;
    snake_x[0] = 80;
    snake_y[0] = 72;
    snake_x[1] = 72;
    snake_y[1] = 72;
    snake_x[2] = 64;
    snake_y[2] = 72;

    current_dir = DIR_RIGHT;

    // Load a sprite tile for the snake’s head/body
    // (You would typically load sprite graphics from .c or .bin files generated by GBTD/GBMB)
    // For example, set_sprite_data(0, 1, snake_tile_data);

    // Initialize snake’s sprite
    // set_sprite_tile(0, 0);
    move_sprite(0, snake_x[0], snake_y[0]);
}

void change_direction(Direction new_dir) {
    // A simple check to prevent reversing directly
    if ((current_dir == DIR_LEFT && new_dir == DIR_RIGHT) ||
        (current_dir == DIR_RIGHT && new_dir == DIR_LEFT) ||
        (current_dir == DIR_UP && new_dir == DIR_DOWN) ||
        (current_dir == DIR_DOWN && new_dir == DIR_UP)) {
        return;
    }
    current_dir = new_dir;
}

void update_snake(void) {
    // Shift body
    for (UINT8 i = snake_length - 1; i > 0; i--) {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    // Move head
    switch(current_dir) {
        case DIR_LEFT:
            snake_x[0] -= 8;
            break;
        case DIR_RIGHT:
            snake_x[0] += 8;
            break;
        case DIR_UP:
            snake_y[0] -= 8;
            break;
        case DIR_DOWN:
            snake_y[0] += 8;
            break;
    }

    // For now, just move one sprite (the head).
    // If you want to visualize the entire snake, you'd set multiple sprites.
    move_sprite(0, snake_x[0], snake_y[0]);
}
