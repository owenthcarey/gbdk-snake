#include <gb/gb.h>
#include <gb/cgb.h> // For Game Boy Color features
#include "snake.h"

#define SNAKE_UPDATE_INTERVAL 10  // Move the snake every 10 frames

void main(void) {
    // Set up the display.
    SHOW_BKG;       // Show the background layer.
    SHOW_SPRITES;   // Enable sprites.
    DISPLAY_ON;     // Turn on the display.

    // Initialize game state.
    init_snake();

    uint8_t frameCounter = 0;

    // Main game loop.
    while(1) {
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

        // Wait until the next vertical blank (frame).
        wait_vbl_done();
    }
}
