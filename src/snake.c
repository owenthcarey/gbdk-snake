#include "snake.h"
#include "save.h"
#include "palettes.h"
#include <string.h>
#include <gb/gb.h>
#include <gb/cgb.h>

/*
 * Note: Warnings like "conditional flow changed by optimizer: so said EVELYN the modified DOG"
 * come from SDCC's optimizer and are harmless.
 */

// Forward declaration to fix implicit declaration warnings.
static void game_over(void);

// Maximum snake length (set this as high as you want; it limits the size of our fixed array)
#define MAX_SNAKE_LENGTH 1000

// Define our playable grid dimensions (each cell is 8x8 pixels)
#define GRID_WIDTH  20
#define GRID_HEIGHT 18

// The snake's positions (in pixels, always multiples of 8)
static uint8_t snake_x[MAX_SNAKE_LENGTH];
static uint8_t snake_y[MAX_SNAKE_LENGTH];
static uint8_t snake_length;
static Direction current_dir;

// Food position (in pixels)
static uint8_t food_x;
static uint8_t food_y;

// Game state
static uint8_t game_over_flag = 0;

// Score tracking
static uint8_t current_score = 0;
static uint8_t high_score = 0;

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
    0xFF,0x81,0xBD,0xA5,0xA5,0xBD,0x81,0xFF,
    0xFF,0xC3,0x99,0xBD,0xBD,0x99,0xC3,0xFF,
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
    0x00,0x00,
    0x18,0x18,
    0x24,0x24,
    0x42,0x42,
    0x42,0x42,
    0x24,0x24,
    0x18,0x18,
    0x00,0x00,
    // Tile 3: Blank (empty cell)
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// Shadow grid of the background (20x18 cells)
static unsigned char bg_grid[GRID_HEIGHT][GRID_WIDTH];

// Color attributes for GBC
static unsigned char bg_attr_grid[GRID_HEIGHT][GRID_WIDTH];

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
// Food Management: Choose a new cell (that isn't occupied by the snake)
// and update the shadow grid accordingly.
// -----------------------------------------------------------------------------
static void spawn_food(void) {
    uint8_t row, col;
    
    // Keep trying random positions until we find an empty cell.
    do {
        // Choose a random position (avoiding the border)
        row = (rand8() % (GRID_HEIGHT - 2)) + 1;
        col = (rand8() % (GRID_WIDTH - 2)) + 1;
    } while (bg_grid[row][col] != 3);  // Keep trying if not empty
    
    // Update the food position.
    food_x = col * 8;
    food_y = row * 8;
    
    // Update the shadow grid.
    bg_grid[row][col] = 2;  // food
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        bg_attr_grid[row][col] = FOOD_PALETTE;
    }
    
    // Update just this one cell on the background.
    set_bkg_tiles(col, row, 1, 1, &bg_grid[row][col]);
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        VBK_REG = 1;  // Switch to attribute bank
        set_bkg_tiles(col, row, 1, 1, &bg_attr_grid[row][col]);
        VBK_REG = 0;  // Switch back to tile bank
    }
}

// -----------------------------------------------------------------------------
// Redraw the entire background from the shadow grid (used at init).
// -----------------------------------------------------------------------------
static void update_entire_bg(void) {
    // Update the entire background from the shadow grid.
    set_bkg_tiles(0, 0, GRID_WIDTH, GRID_HEIGHT, &bg_grid[0][0]);
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        VBK_REG = 1;  // Switch to attribute bank
        set_bkg_tiles(0, 0, GRID_WIDTH, GRID_HEIGHT, &bg_attr_grid[0][0]);
        VBK_REG = 0;  // Switch back to tile bank
    }
}

// -----------------------------------------------------------------------------
// Initialize the snake game
// -----------------------------------------------------------------------------
void init_snake(void) {
    // Load our 4 background tiles into VRAM (tile indices 0..3)
    set_bkg_data(0, 4, bg_tiles);

    // Initialize the shadow grid to blank (tile 3)
    for(uint8_t row = 0; row < GRID_HEIGHT; row++) {
        for(uint8_t col = 0; col < GRID_WIDTH; col++) {
            bg_grid[row][col] = 3;
            
            // Set color attributes for GBC
            if (_cpu == CGB_TYPE) {
                // Border cells get the border palette
                if (row == 0 || row == GRID_HEIGHT-1 || col == 0 || col == GRID_WIDTH-1) {
                    bg_attr_grid[row][col] = BORDER_PALETTE;
                } else {
                    // Interior cells get the background palette
                    bg_attr_grid[row][col] = BG_PALETTE_GREEN;
                }
            }
        }
    }

    // Set initial snake (length 3) at a center-ish position.
    snake_length = 3;
    snake_x[0] = 80;  snake_y[0] = 72;  // head
    snake_x[1] = 72;  snake_y[1] = 72;
    snake_x[2] = 64;  snake_y[2] = 72;
    current_dir = DIR_RIGHT;

    // Update the snake's cells in the shadow grid.
    for(uint8_t i = 0; i < snake_length; i++) {
        uint8_t col = snake_x[i] / 8;
        uint8_t row = snake_y[i] / 8;
        bg_grid[row][col] = (i == 0) ? 0 : 1;
        
        // Set color attributes for GBC
        if (_cpu == CGB_TYPE) {
            bg_attr_grid[row][col] = SNAKE_PALETTE;
        }
    }

    // Spawn initial food (which updates its cell in the shadow grid)
    spawn_food();

    // Draw the entire background from the shadow grid.
    update_entire_bg();

    // Reset game over flag and current score
    game_over_flag = 0;
    current_score = 0;
    
    // Load high score from SRAM
    if (has_save_data()) {
        high_score = load_high_score();
    } else {
        high_score = 0;
        // Initialize save data with a high score of 0
        save_high_score(0);
    }
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
// Check if the snake collides with itself
// -----------------------------------------------------------------------------
static uint8_t snake_collides_with_self(void) {
    // Check if the head collides with any part of the body
    for (uint8_t i = 1; i < snake_length; i++) {
        if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
            return 1;  // Collision detected
        }
    }
    return 0;  // No collision
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

    // Check if the snake collides with itself
    if (snake_collides_with_self()) {
        game_over();
        return;
    }

    // Determine if the snake has eaten the food.
    uint8_t ate_food = (snake_x[0] == food_x && snake_y[0] == food_y);

    if (ate_food) {
        // Increase score
        current_score++;
        
        // Update high score if needed
        update_high_score();
        
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
            
            // Set color attributes for GBC
            if (_cpu == CGB_TYPE) {
                bg_attr_grid[tail_row][tail_col] = BG_PALETTE_GREEN;
            }
            
            set_bkg_tiles(tail_col, tail_row, 1, 1, &bg_grid[tail_row][tail_col]);
            
            // Set color attributes for GBC
            if (_cpu == CGB_TYPE) {
                VBK_REG = 1;  // Switch to attribute bank
                set_bkg_tiles(tail_col, tail_row, 1, 1, &bg_attr_grid[tail_row][tail_col]);
                VBK_REG = 0;  // Switch back to tile bank
            }
        }
        // Spawn new food (updates its cell in the shadow grid)
        spawn_food();
    } else {
        // Not eating food: clear the old tail cell.
        uint8_t tail_col = old_tail_x / 8;
        uint8_t tail_row = old_tail_y / 8;
        bg_grid[tail_row][tail_col] = 3;  // blank
        
        // Set color attributes for GBC
        if (_cpu == CGB_TYPE) {
            bg_attr_grid[tail_row][tail_col] = BG_PALETTE_GREEN;
        }
        
        set_bkg_tiles(tail_col, tail_row, 1, 1, &bg_grid[tail_row][tail_col]);
        
        // Set color attributes for GBC
        if (_cpu == CGB_TYPE) {
            VBK_REG = 1;  // Switch to attribute bank
            set_bkg_tiles(tail_col, tail_row, 1, 1, &bg_attr_grid[tail_row][tail_col]);
            VBK_REG = 0;  // Switch back to tile bank
        }
    }

    // The old head cell now becomes a body cell.
    uint8_t old_head_col = old_head_x / 8;
    uint8_t old_head_row = old_head_y / 8;
    bg_grid[old_head_row][old_head_col] = 1;  // snake body
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        bg_attr_grid[old_head_row][old_head_col] = SNAKE_PALETTE;
    }
    
    set_bkg_tiles(old_head_col, old_head_row, 1, 1, &bg_grid[old_head_row][old_head_col]);
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        VBK_REG = 1;  // Switch to attribute bank
        set_bkg_tiles(old_head_col, old_head_row, 1, 1, &bg_attr_grid[old_head_row][old_head_col]);
        VBK_REG = 0;  // Switch back to tile bank
    }

    // The new head cell is drawn as the head.
    uint8_t new_head_col = snake_x[0] / 8;
    uint8_t new_head_row = snake_y[0] / 8;
    bg_grid[new_head_row][new_head_col] = 0;  // snake head
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        bg_attr_grid[new_head_row][new_head_col] = SNAKE_PALETTE;
    }
    
    set_bkg_tiles(new_head_col, new_head_row, 1, 1, &bg_grid[new_head_row][new_head_col]);
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        VBK_REG = 1;  // Switch to attribute bank
        set_bkg_tiles(new_head_col, new_head_row, 1, 1, &bg_attr_grid[new_head_row][new_head_col]);
        VBK_REG = 0;  // Switch back to tile bank
    }
}

// -----------------------------------------------------------------------------
// Game Over: Set the game over flag
// -----------------------------------------------------------------------------
static void game_over(void) {
    game_over_flag = 1;
    
    // Save high score when game ends
    if (current_score > high_score) {
        high_score = current_score;
        // Make sure to save the high score to SRAM
        save_high_score(high_score);
    }
}

// Public function to check if the game is over
uint8_t is_game_over(void) {
    return game_over_flag;
}

// Get the current score
uint8_t get_score(void) {
    return current_score;
}

// Get the high score
uint8_t get_high_score(void) {
    return high_score;
}

// Update the high score if the current score is higher
void update_high_score(void) {
    if (current_score > high_score) {
        high_score = current_score;
        save_high_score(high_score);
    }
}
