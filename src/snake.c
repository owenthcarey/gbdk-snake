#include "snake.h"
#include <string.h>

// Maximum allowed length for the snake.
#define MAX_SNAKE_LENGTH 20
// We use sprite index MAX_SNAKE_LENGTH for the food.
#define FOOD_SPRITE_INDEX MAX_SNAKE_LENGTH

// Offsets to map logical coordinates (multiples of 8) to hardware sprite positions.
#define X_OFFSET 8
#define Y_OFFSET 16

// Logical snake segment positions (in 8-pixel steps).
static uint8_t snake_x[MAX_SNAKE_LENGTH];
static uint8_t snake_y[MAX_SNAKE_LENGTH];
static uint8_t snake_length;
static Direction current_dir;

// Logical food position.
static uint8_t food_x;
static uint8_t food_y;

// -----------------------------------------------------------------------------
// Tile Data (each 8x8 tile is 16 bytes in 2bpp format)
// -----------------------------------------------------------------------------

// Snake head tile (tile 0)
static const unsigned char snake_head_tile[] = {
    0xFF,0x81,0xBD,0xA5,
    0xA5,0xBD,0x81,0xFF,
    0xFF,0xC3,0x99,0xBD,
    0xBD,0x99,0xC3,0xFF
};

// Snake body tile (tile 1)
static const unsigned char snake_body_tile[] = {
    0x00,0x00,
    0x3C,0x3C,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x3C,0x3C,
    0x00,0x00
};

// Food tile (tile 2)
static const unsigned char food_tile[] = {
    0x3C,0x3C,
    0x42,0x42,
    0x99,0x99,
    0xA5,0xA5,
    0xA5,0xA5,
    0x99,0x99,
    0x42,0x42,
    0x3C,0x3C
};

// -----------------------------------------------------------------------------
// Utility: Simple pseudo-random generator.
// -----------------------------------------------------------------------------
static uint8_t rand8(void) {
    static uint16_t seed = 1234;  // Initial seed value.
    seed = seed * 1103515245 + 12345;
    return (seed >> 8) & 0xFF;
}

// Check if the given (x,y) collides with any snake segment.
static uint8_t snake_collides(uint8_t x, uint8_t y) {
    for(uint8_t i = 0; i < snake_length; i++) {
        if(snake_x[i] == x && snake_y[i] == y)
            return 1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Food Management
// -----------------------------------------------------------------------------
static void spawn_food(void) {
    uint8_t valid = 0;
    while(!valid) {
        // Our logical grid:
        // For X: 20 columns (0 to 19) => positions 0 to 19*8 = 152.
        // For Y: 18 rows (0 to 17)  => positions 0 to 17*8 = 136.
        food_x = (rand8() % 20) * 8;
        food_y = (rand8() % 18) * 8;
        if (!snake_collides(food_x, food_y)) {
            valid = 1;
        }
    }
    // Assign the food tile (tile 2) to its sprite.
    set_sprite_tile(FOOD_SPRITE_INDEX, 2);
    move_sprite(FOOD_SPRITE_INDEX, food_x + X_OFFSET, food_y + Y_OFFSET);
}

// -----------------------------------------------------------------------------
// Game Over: For now, simply restart the game.
// -----------------------------------------------------------------------------
static void game_over(void) {
    // (Optionally add game-over effects here, like flashing the screen.)
    init_snake();
}

// -----------------------------------------------------------------------------
// Initialize the snake and game state.
// -----------------------------------------------------------------------------
void init_snake(void) {
    // Load tile data into VRAM:
    // Load snake head into tile 0.
    set_sprite_data(0, 1, snake_head_tile);
    // Load snake body into tile 1.
    set_sprite_data(1, 1, snake_body_tile);
    // Load food tile into tile 2.
    set_sprite_data(2, 1, food_tile);

    // Initialize snake starting position with length 3.
    // (Logical positions in multiples of 8.)
    snake_length = 3;
    snake_x[0] = 80;  // Head (center-ish)
    snake_y[0] = 72;
    snake_x[1] = 72;
    snake_y[1] = 72;
    snake_x[2] = 64;
    snake_y[2] = 72;

    current_dir = DIR_RIGHT;

    // Set up the snake sprites.
    for (uint8_t i = 0; i < snake_length; i++) {
        // Use tile 0 for head; tile 1 for body segments.
        set_sprite_tile(i, (i == 0) ? 0 : 1);
        move_sprite(i, snake_x[i] + X_OFFSET, snake_y[i] + Y_OFFSET);
    }

    // Hide any unused snake sprites (move them offscreen).
    for (uint8_t i = snake_length; i < MAX_SNAKE_LENGTH; i++) {
        move_sprite(i, 0, 0);
    }

    // Spawn the initial food.
    spawn_food();
}

// -----------------------------------------------------------------------------
// Change the snake's direction (prevents 180° reversal).
// -----------------------------------------------------------------------------
void change_direction(Direction new_dir) {
    if ((current_dir == DIR_LEFT && new_dir == DIR_RIGHT) ||
        (current_dir == DIR_RIGHT && new_dir == DIR_LEFT) ||
        (current_dir == DIR_UP && new_dir == DIR_DOWN) ||
        (current_dir == DIR_DOWN && new_dir == DIR_UP)) {
        return;
    }
    current_dir = new_dir;
}

// -----------------------------------------------------------------------------
// Update the snake's position, check for collisions, and update sprites.
// -----------------------------------------------------------------------------
void update_snake(void) {
    // Move the snake body: each segment takes the position of the previous one.
    for (uint8_t i = snake_length - 1; i > 0; i--) {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    // Update the head based on the current direction.
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

    // -------------------------------------------------------------------------
    // Collision Detection: Check for boundary hits.
    // Our logical grid spans:
    //   X: 0 to 152, Y: 0 to 136.
    // Note: Underflow (going negative) wraps to a high value.
    // -------------------------------------------------------------------------
    if (snake_x[0] > 152 || snake_y[0] > 136) {
        game_over();
        return;
    }

    // -------------------------------------------------------------------------
    // Food and Growth: If the snake eats the food, grow and spawn new food.
    // -------------------------------------------------------------------------
    if (snake_x[0] == food_x && snake_y[0] == food_y) {
        if (snake_length < MAX_SNAKE_LENGTH) {
            // Add a new segment at the tail (duplicate the last segment's position).
            snake_x[snake_length] = snake_x[snake_length - 1];
            snake_y[snake_length] = snake_y[snake_length - 1];
            snake_length++;
        }
        spawn_food();
    }

    // -------------------------------------------------------------------------
    // Update the sprite positions for the entire snake.
    // -------------------------------------------------------------------------
    for (uint8_t i = 0; i < snake_length; i++) {
        // Head uses tile 0; body segments use tile 1.
        set_sprite_tile(i, (i == 0) ? 0 : 1);
        move_sprite(i, snake_x[i] + X_OFFSET, snake_y[i] + Y_OFFSET);
    }
}
