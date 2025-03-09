#include "title.h"
#include <gb/gb.h>
#include <string.h>

// Title screen tile data (16 tiles total)
const unsigned char title_tiles[] = {
    // Tile 0: Empty space
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // Tile 1: Border tile
    0xFF, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0xFF,
    
    // Tile 2: Snake head
    0xFF, 0x81, 0xBD, 0xA5, 0xA5, 0xBD, 0x81, 0xFF,
    0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF,
    
    // Tile 3: Letter S
    0x3C, 0x3C, 0x42, 0x42, 0x40, 0x40, 0x3C, 0x3C,
    0x02, 0x02, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00,
    
    // Tile 4: Letter N
    0x42, 0x42, 0x62, 0x62, 0x52, 0x52, 0x4A, 0x4A,
    0x46, 0x46, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
    
    // Tile 5: Letter A
    0x3C, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x7E,
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
    
    // Tile 6: Letter K
    0x42, 0x42, 0x44, 0x44, 0x48, 0x48, 0x70, 0x70,
    0x48, 0x48, 0x44, 0x44, 0x42, 0x42, 0x00, 0x00,
    
    // Tile 7: Letter E
    0x7E, 0x7E, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x7C,
    0x40, 0x40, 0x40, 0x40, 0x7E, 0x7E, 0x00, 0x00,
    
    // Tile 8: Letter G
    0x3C, 0x3C, 0x42, 0x42, 0x40, 0x40, 0x40, 0x40,
    0x4E, 0x4E, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00,
    
    // Tile 9: Letter M
    0x42, 0x42, 0x66, 0x66, 0x5A, 0x5A, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
    
    // Tile 10: Letter P
    0x7C, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x7C,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00,
    
    // Tile 11: Letter R
    0x7C, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x7C,
    0x48, 0x48, 0x44, 0x44, 0x42, 0x42, 0x00, 0x00,
    
    // Tile 12: Letter T
    0x7E, 0x7E, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    
    // Tile 13: Letter B
    0x7C, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x7C,
    0x42, 0x42, 0x42, 0x42, 0x7C, 0x7C, 0x00, 0x00,
    
    // Tile 14: Letter I
    0x3E, 0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x3E, 0x3E, 0x00, 0x00,
    
    // Tile 15: Letter O
    0x3C, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00
};

// Title screen map - border with empty space inside
const unsigned char title_border_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

// "SNAKE GAME" title text
const unsigned char title_text[] = {3, 4, 5, 6, 7, 0, 8, 5, 9, 7};

// "PRESS START" text
const unsigned char press_start_text[] = {10, 11, 7, 3, 3, 0, 3, 12, 5, 11, 12};

// "TO BEGIN" text
const unsigned char to_begin_text[] = {12, 15, 0, 13, 7, 8, 14, 4};

// Animation variables
static uint8_t animation_frame = 0;
static uint8_t animation_delay = 0;

void init_title_screen(void) {
    // Load title screen tiles into VRAM
    set_bkg_data(0, 16, title_tiles);
    
    // Set up the border
    set_bkg_tiles(0, 0, 20, 18, title_border_map);
    
    // Draw the title text "SNAKE GAME" at row 4
    set_bkg_tiles(5, 4, 10, 1, title_text);
    
    // Draw "PRESS START" at row 10
    set_bkg_tiles(5, 10, 11, 1, press_start_text);
    
    // Draw "TO BEGIN" at row 12
    set_bkg_tiles(6, 12, 8, 1, to_begin_text);
    
    // Draw a snake head for decoration
    set_bkg_tiles(10, 7, 1, 1, &title_tiles[2]);
}

uint8_t update_title_screen(void) {
    // Simple animation effect - blink the "PRESS START" text
    animation_delay++;
    if (animation_delay >= 30) {  // Every half second (30 frames)
        animation_delay = 0;
        animation_frame = !animation_frame;
        
        if (animation_frame) {
            // Hide the text by replacing with empty tiles
            unsigned char empty_tiles[11];
            memset(empty_tiles, 0, 11);
            set_bkg_tiles(5, 10, 11, 1, empty_tiles);
        } else {
            // Show the text
            set_bkg_tiles(5, 10, 11, 1, press_start_text);
        }
    }
    
    // Check for START button press
    uint8_t keys = joypad();
    if (keys & J_START) {
        return 1;  // Return 1 to indicate the game should start
    }
    
    return 0;  // Continue showing the title screen
} 