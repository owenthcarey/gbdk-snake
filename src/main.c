#include <gb/gb.h>
#include <gb/cgb.h> // For Game Boy Color features
#include <string.h>
#include "snake.h"
#include "title.h"

#define SNAKE_UPDATE_INTERVAL 10  // Move the snake every 10 frames

// Game states
#define STATE_TITLE 0
#define STATE_PLAYING 1
#define STATE_GAME_OVER 2

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

void main(void) {
    // Set up the display.
    SHOW_BKG;       // Show the background layer.
    SHOW_SPRITES;   // Enable sprites.
    DISPLAY_ON;     // Turn on the display.

    // Start with the title screen
    uint8_t game_state = STATE_TITLE;
    init_title_screen();

    uint8_t frameCounter = 0;
    uint8_t game_over_delay = 0;

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
