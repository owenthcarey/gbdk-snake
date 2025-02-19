// #include <stdio.h>
//
// int main(void) {
//     printf("Hello, World!\n");
//     return 0;
// }

#include <gb/gb.h>
#include <gb/cgb.h>   // For Game Boy Color features
#include "snake.h"

#define SNAKE_UPDATE_INTERVAL 10  // Move the snake every 10 frames

void main(void) {
    // Initialize Game Boy
    // set GBC Mode background color if desired
    // Use set_default_palette() if you want default GBC palette
    // or define your own with set_bkg_palette(0, 1, palette)

    SHOW_BKG;  // Show the background layer
    SHOW_SPRITES; // Show sprites
    DISPLAY_ON; // Turn on the display

    init_snake(); // Initialize snake data

    uint8_t frameCounter = 0;

    // Main loop
    while(1) {
        // Read Joypad
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

        // Update snake
        // update_snake();

        // Only move the snake every SNAKE_UPDATE_INTERVAL frames
        if (++frameCounter >= SNAKE_UPDATE_INTERVAL) {
            update_snake();
            frameCounter = 0;  // reset counter
        }

        // Wait for the next frame
        wait_vbl_done();
    }
}
