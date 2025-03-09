#include <gb/gb.h>
#include <gb/cgb.h> // For Game Boy Color features
#include <string.h>
#include "snake.h"
#include "title.h"
#include "save.h"
#include "palettes.h"

#define SNAKE_UPDATE_INTERVAL 10  // Move the snake every 10 frames

// Game states
#define STATE_TITLE 0
#define STATE_PLAYING 1
#define STATE_GAME_OVER 2

// Number tiles for score display (0-9)
const unsigned char number_tiles[] = {
    // Tile 0: Number 0
    0x3C, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00,
    
    // Tile 1: Number 1
    0x08, 0x08, 0x18, 0x18, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x1C, 0x1C, 0x00, 0x00,
    
    // Tile 2: Number 2
    0x3C, 0x3C, 0x42, 0x42, 0x02, 0x02, 0x0C, 0x0C,
    0x30, 0x30, 0x40, 0x40, 0x7E, 0x7E, 0x00, 0x00,
    
    // Tile 3: Number 3
    0x3C, 0x3C, 0x42, 0x42, 0x02, 0x02, 0x1C, 0x1C,
    0x02, 0x02, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00,
    
    // Tile 4: Number 4
    0x04, 0x04, 0x0C, 0x0C, 0x14, 0x14, 0x24, 0x24,
    0x44, 0x44, 0x7E, 0x7E, 0x04, 0x04, 0x00, 0x00,
    
    // Tile 5: Number 5
    0x7E, 0x7E, 0x40, 0x40, 0x7C, 0x7C, 0x02, 0x02,
    0x02, 0x02, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00,
    
    // Tile 6: Number 6
    0x3C, 0x3C, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x7C,
    0x42, 0x42, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00,
    
    // Tile 7: Number 7
    0x7E, 0x7E, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    
    // Tile 8: Number 8
    0x3C, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x3C,
    0x42, 0x42, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00,
    
    // Tile 9: Number 9
    0x3C, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3E, 0x3E,
    0x02, 0x02, 0x02, 0x02, 0x3C, 0x3C, 0x00, 0x00
};

// Text for "SCORE:" and "HI:"
const unsigned char score_text[] = {
    // S, C, O, R, E, :
    0x3C, 0x3C, 0x42, 0x42, 0x40, 0x40, 0x3C, 0x3C, 0x02, 0x02, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00, // S
    0x3C, 0x3C, 0x42, 0x42, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00, // C
    0x3C, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00, // O
    0x7C, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x7C, 0x48, 0x48, 0x44, 0x44, 0x42, 0x42, 0x00, 0x00, // R
    0x7E, 0x7E, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x7E, 0x00, 0x00, // E
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00  // :
};

const unsigned char hi_text[] = {
    // H, I, :
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, // H
    0x3E, 0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x3E, 0x00, 0x00, // I
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00  // :
};

// Game over text tiles (G, A, M, E, O, V, E, R)
const unsigned char game_over_text[] = {
    0x3C, 0x3C, 0x42, 0x42, 0x40, 0x40, 0x40, 0x40, 0x4E, 0x4E, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00, // G
    0x3C, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, // A
    0x42, 0x42, 0x66, 0x66, 0x5A, 0x5A, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, // M
    0x7E, 0x7E, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x7E, 0x00, 0x00, // E
    0x3C, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00, // O
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x24, 0x24, 0x24, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, // V
    0x7E, 0x7E, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x7E, 0x00, 0x00, // E
    0x7C, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x7C, 0x48, 0x48, 0x44, 0x44, 0x42, 0x42, 0x00, 0x00  // R
};

// Game over tile indices
const unsigned char game_over_indices[] = {0, 1, 2, 3, 4, 5, 3, 6};

// Function to display the score
void display_score(void) {
    // Load score text tiles into VRAM at position 24
    set_bkg_data(24, 6, score_text);
    
    // Display "SCORE:" text at top of screen
    unsigned char score_indices[] = {24, 25, 26, 27, 28, 29};
    set_bkg_tiles(1, 0, 6, 1, score_indices);
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        unsigned char text_attrs[6];
        memset(text_attrs, TEXT_PALETTE, 6);
        
        VBK_REG = 1;  // Switch to attribute bank
        set_bkg_tiles(1, 0, 6, 1, text_attrs);
        VBK_REG = 0;  // Switch back to tile bank
    }
    
    // Get current score
    uint8_t score = get_score();
    
    // Convert score to digits
    unsigned char score_digits[2];
    score_digits[0] = score / 10;  // Tens digit
    score_digits[1] = score % 10;  // Ones digit
    
    // Load number tiles into VRAM at position 40
    set_bkg_data(40, 10, number_tiles);
    
    // Display score digits
    unsigned char display_digits[2];
    display_digits[0] = 40 + score_digits[0];
    display_digits[1] = 40 + score_digits[1];
    set_bkg_tiles(8, 0, 2, 1, display_digits);
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        unsigned char digit_attrs[2];
        memset(digit_attrs, TEXT_PALETTE, 2);
        
        VBK_REG = 1;  // Switch to attribute bank
        set_bkg_tiles(8, 0, 2, 1, digit_attrs);
        VBK_REG = 0;  // Switch back to tile bank
    }
}

// Function to display the high score
void display_high_score(void) {
    // Load high score text tiles into VRAM at position 30
    set_bkg_data(30, 3, hi_text);
    
    // Display "HI:" text at top of screen
    unsigned char hi_indices[] = {30, 31, 32};
    set_bkg_tiles(12, 0, 3, 1, hi_indices);
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        unsigned char text_attrs[3];
        memset(text_attrs, TEXT_PALETTE, 3);
        
        VBK_REG = 1;  // Switch to attribute bank
        set_bkg_tiles(12, 0, 3, 1, text_attrs);
        VBK_REG = 0;  // Switch back to tile bank
    }
    
    // Get high score
    uint8_t hi_score = get_high_score();
    
    // Convert high score to digits
    unsigned char hi_score_digits[2];
    hi_score_digits[0] = hi_score / 10;  // Tens digit
    hi_score_digits[1] = hi_score % 10;  // Ones digit
    
    // Display high score digits
    unsigned char display_digits[2];
    display_digits[0] = 40 + hi_score_digits[0];
    display_digits[1] = 40 + hi_score_digits[1];
    set_bkg_tiles(16, 0, 2, 1, display_digits);
    
    // Set color attributes for GBC
    if (_cpu == CGB_TYPE) {
        unsigned char digit_attrs[2];
        memset(digit_attrs, TEXT_PALETTE, 2);
        
        VBK_REG = 1;  // Switch to attribute bank
        set_bkg_tiles(16, 0, 2, 1, digit_attrs);
        VBK_REG = 0;  // Switch back to tile bank
    }
}

void main(void) {
    // Set up the display.
    SHOW_BKG;       // Show the background layer.
    SHOW_SPRITES;   // Enable sprites.
    DISPLAY_ON;     // Turn on the display.
    
    // Initialize color palettes for GBC
    if (_cpu == CGB_TYPE) {
        init_palettes();
    }

    // Initialize high score from SRAM at startup
    if (has_save_data()) {
        uint8_t saved_score = load_high_score();
        // We don't need to do anything with the score here,
        // it will be loaded in init_snake() when the game starts
    }

    // Start with the title screen
    uint8_t game_state = STATE_TITLE;
    init_title_screen();

    uint8_t frameCounter = 0;
    uint8_t game_over_delay = 0;
    uint8_t score_update_counter = 0;

    // Main game loop.
    while(1) {
        // Handle different game states
        if (game_state == STATE_TITLE) {
            // Update title screen and check if we should start the game
            if (update_title_screen()) {
                // Switch to playing state
                game_state = STATE_PLAYING;
                
                // Initialize game state
                init_snake();
                
                // Display initial score and high score
                display_score();
                display_high_score();
            }
        } else if (game_state == STATE_PLAYING) {
            // Check if game is over
            if (is_game_over()) {
                game_state = STATE_GAME_OVER;
                game_over_delay = 0;
                
                // Load game over tiles
                set_bkg_data(16, 8, game_over_text);
                
                // Display "GAME OVER" text
                unsigned char display_indices[8];
                for (uint8_t i = 0; i < 8; i++) {
                    display_indices[i] = 16 + game_over_indices[i];
                }
                set_bkg_tiles(6, 8, 8, 1, display_indices);
                
                // Set color attributes for GBC
                if (_cpu == CGB_TYPE) {
                    unsigned char gameover_attrs[8];
                    memset(gameover_attrs, GAMEOVER_PALETTE, 8);
                    
                    VBK_REG = 1;  // Switch to attribute bank
                    set_bkg_tiles(6, 8, 8, 1, gameover_attrs);
                    VBK_REG = 0;  // Switch back to tile bank
                }
                
                // Update high score display one last time
                display_high_score();
                
                continue;
            }
            
            // Read the joypad for direction input.
            uint8_t keys = joypad();
            if (keys & J_LEFT) {
                change_direction(DIR_LEFT);
            } else if (keys & J_RIGHT) {
                change_direction(DIR_RIGHT);
            } else if (keys & J_UP) {
                change_direction(DIR_UP);
            } else if (keys & J_DOWN) {
                change_direction(DIR_DOWN);
            }

            // Update the snake only every few frames to control speed.
            if (++frameCounter >= SNAKE_UPDATE_INTERVAL) {
                update_snake();
                frameCounter = 0;
                
                // Update score display every few snake updates
                if (++score_update_counter >= 3) {
                    display_score();
                    display_high_score();
                    score_update_counter = 0;
                }
            }
        } else if (game_state == STATE_GAME_OVER) {
            // Wait a moment before returning to title screen
            if (++game_over_delay >= 120) {  // About 2 seconds
                // Return to title screen
                game_state = STATE_TITLE;
                init_title_screen();
            }
            
            // Also allow player to press start to continue
            uint8_t keys = joypad();
            if (keys & J_START) {
                game_state = STATE_TITLE;
                init_title_screen();
            }
        }

        // Wait until the next vertical blank (frame).
        wait_vbl_done();
    }
}
