#include "snake.h"
#include <string.h>

/*
 * Note: Warnings like "conditional flow changed by optimizer: so said EVELYN the modified DOG"
 * come from SDCC’s optimizer and are harmless.
 */

// Forward declaration to fix implicit declaration warnings.
static void game_over(void);

// Maximum snake length (set this as high as you want; it limits the size of our fixed array)
#define MAX_SNAKE_LENGTH 1000

// Define our playable grid dimensions (each cell is 8x8 pixels)
#define GRID_WIDTH  20
#define GRID_HEIGHT 18

// The snake’s positions (in pixels, always multiples of 8)
static uint8_t snake_x[MAX_SNAKE_LENGTH];
static uint8_t snake_y[MAX_SNAKE_LENGTH];
static uint8_t snake_length;
static Direction current_dir;

// Food position (in pixels)
static uint8_t food_x;
static uint8_t food_y;

// -----------------------------------------------------------------------------
// Background Tile Indices
// We use 4 tiles (loaded into VRAM):
//   0: Snake head
//   1: Snake body
//   2: Food
//   3: Blank (empty cell)
// -----------------------------------------------------------------------------

// The 4 tiles (each tile is 16 bytes in 2bpp format)
static const unsigned char bg_tiles[64] = {
    // Tile 0: Snake head
    0xFF,0x81,0xBD,0xA5,
    0xA5,0xBD,0x81,0xFF,
    0xFF,0xC3,0x99,0xBD,
    0xBD,0x99,0xC3,0xFF,
    // Tile 1: Snake body
    0x00,0x00,
    0x3C,0x3C,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x3C,0x3C,
    0x00,0x00,
    // Tile 2: Food
    0x3C,0x3C,
    0x42,0x42,
    0x99,0x99,
    0xA5,0xA5,
    0xA5,0xA5,
    0x99,0x99,
    0x42,0x42,
    0x3C,0x3C,
    // Tile 3: Blank
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

// -----------------------------------------------------------------------------
// Shadow grid: our internal representation of the playable area.
// Each cell holds the tile index (0..3) that should be drawn.
// -----------------------------------------------------------------------------
static UBYTE bg_grid[GRID_HEIGHT][GRID_WIDTH];

// -----------------------------------------------------------------------------
// Utility: Simple pseudo-random generator.
// -----------------------------------------------------------------------------
static uint8_t rand8(void) {
    static uint16_t seed = 1234;  // initial seed
    seed = seed * 1103515245 + 12345;
    return (seed >> 8) & 0xFF;
}

// Check if the given pixel position collides with any snake segment.
static uint8_t snake_collides(uint8_t x, uint8_t y) {
    for(uint8_t i = 0; i < snake_length; i++) {
        if(snake_x[i] == x && snake_y[i] == y)
            return 1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Food Management: Choose a new cell (that isn’t occupied by the snake)
// and update the shadow grid accordingly.
// -----------------------------------------------------------------------------
static void spawn_food(void) {
    uint8_t valid = 0;
    while(!valid) {
        food_x = (rand8() % GRID_WIDTH) * 8;
        food_y = (rand8() % GRID_HEIGHT) * 8;
        if (!snake_collides(food_x, food_y))
            valid = 1;
    }
    // Update the cell in the shadow grid
    uint8_t col = food_x / 8;
    uint8_t row = food_y / 8;
    bg_grid[row][col] = 2;  // Food tile
    set_bkg_tiles(col, row, 1, 1, &bg_grid[row][col]);
}

// -----------------------------------------------------------------------------
// Redraw the entire background from the shadow grid (used at init).
// -----------------------------------------------------------------------------
static void update_entire_bg(void) {
    set_bkg_tiles(0, 0, GRID_WIDTH, GRID_HEIGHT, &bg_grid[0][0]);
}

// -----------------------------------------------------------------------------
// Initialize the snake and game state.
// -----------------------------------------------------------------------------
void init_snake(void) {
    // Load our 4 background tiles into VRAM (tile indices 0..3)
    set_bkg_data(0, 4, bg_tiles);

    // Initialize the shadow grid to blank (tile 3)
    for(uint8_t row = 0; row < GRID_HEIGHT; row++) {
        for(uint8_t col = 0; col < GRID_WIDTH; col++) {
            bg_grid[row][col] = 3;
        }
    }

    // Set initial snake (length 3) at a center-ish position.
    snake_length = 3;
    snake_x[0] = 80;  snake_y[0] = 72;  // head
    snake_x[1] = 72;  snake_y[1] = 72;
    snake_x[2] = 64;  snake_y[2] = 72;
    current_dir = DIR_RIGHT;

    // Update the snake’s cells in the shadow grid.
    for(uint8_t i = 0; i < snake_length; i++) {
        uint8_t col = snake_x[i] / 8;
        uint8_t row = snake_y[i] / 8;
        bg_grid[row][col] = (i == 0) ? 0 : 1;
    }

    // Spawn initial food (which updates its cell in the shadow grid)
    spawn_food();

    // Draw the entire background from the shadow grid.
    update_entire_bg();
}

// -----------------------------------------------------------------------------
// Change the snake's direction (ignores 180° reversals).
// -----------------------------------------------------------------------------
void change_direction(Direction new_dir) {
    if ((current_dir == DIR_LEFT && new_dir == DIR_RIGHT) ||
        (current_dir == DIR_RIGHT && new_dir == DIR_LEFT) ||
        (current_dir == DIR_UP && new_dir == DIR_DOWN) ||
        (current_dir == DIR_DOWN && new_dir == DIR_UP))
        return;
    current_dir = new_dir;
}

// -----------------------------------------------------------------------------
// Incrementally update only the cells that change when the snake moves.
// -----------------------------------------------------------------------------
void update_snake(void) {
    // Record the old tail cell (only used if we are not growing)
    uint8_t old_tail_x = snake_x[snake_length - 1];
    uint8_t old_tail_y = snake_y[snake_length - 1];
    // Record the old head cell position (which will become a body segment)
    uint8_t old_head_x = snake_x[0];
    uint8_t old_head_y = snake_y[0];

    // Shift snake segments (from tail to head)
    for (uint8_t i = snake_length - 1; i > 0; i--) {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    // Update the head based on the current direction.
    switch(current_dir) {
        case DIR_LEFT:
            if(snake_x[0] < 8) { game_over(); return; }
            snake_x[0] -= 8;
            break;
        case DIR_RIGHT:
            if(snake_x[0] >= (GRID_WIDTH - 1) * 8) { game_over(); return; }
            snake_x[0] += 8;
            break;
        case DIR_UP:
            if(snake_y[0] < 8) { game_over(); return; }
            snake_y[0] -= 8;
            break;
        case DIR_DOWN:
            if(snake_y[0] >= (GRID_HEIGHT - 1) * 8) { game_over(); return; }
            snake_y[0] += 8;
            break;
    }

    // Determine if the snake has eaten the food.
    uint8_t ate_food = (snake_x[0] == food_x && snake_y[0] == food_y);

    if (ate_food) {
        if(snake_length < MAX_SNAKE_LENGTH) {
            // Grow the snake by duplicating the old tail cell.
            snake_x[snake_length] = old_tail_x;
            snake_y[snake_length] = old_tail_y;
            snake_length++;
        } else {
            // At max length: behave like normal movement and clear the tail.
            uint8_t tail_col = old_tail_x / 8;
            uint8_t tail_row = old_tail_y / 8;
            bg_grid[tail_row][tail_col] = 3;  // blank
            set_bkg_tiles(tail_col, tail_row, 1, 1, &bg_grid[tail_row][tail_col]);
        }
        // Spawn new food (updates its cell in the shadow grid)
        spawn_food();
    } else {
        // Not eating food: clear the old tail cell.
        uint8_t tail_col = old_tail_x / 8;
        uint8_t tail_row = old_tail_y / 8;
        bg_grid[tail_row][tail_col] = 3;  // blank
        set_bkg_tiles(tail_col, tail_row, 1, 1, &bg_grid[tail_row][tail_col]);
    }

    // The old head cell now becomes a body cell.
    uint8_t old_head_col = old_head_x / 8;
    uint8_t old_head_row = old_head_y / 8;
    bg_grid[old_head_row][old_head_col] = 1;  // snake body
    set_bkg_tiles(old_head_col, old_head_row, 1, 1, &bg_grid[old_head_row][old_head_col]);

    // The new head cell is drawn as the head.
    uint8_t new_head_col = snake_x[0] / 8;
    uint8_t new_head_row = snake_y[0] / 8;
    bg_grid[new_head_row][new_head_col] = 0;  // snake head
    set_bkg_tiles(new_head_col, new_head_row, 1, 1, &bg_grid[new_head_row][new_head_col]);
}

// -----------------------------------------------------------------------------
// Game Over: Simply restart the game.
// -----------------------------------------------------------------------------
static void game_over(void) {
    init_snake();
}
