#include "palettes.h"
#include <gb/gb.h>
#include <gb/cgb.h>

// Background palette - High contrast light theme
const UWORD bg_palette_green[] = {
    RGB(248, 248, 248),  // Almost white background (much lighter)
    RGB(180, 220, 180),  // Light green
    RGB(80, 160, 80),    // Medium green (higher contrast)
    RGB(20, 80, 20)      // Very dark green (higher contrast)
};

// Snake palette - High contrast blue theme
const UWORD snake_palette[] = {
    RGB(248, 248, 248),  // Almost white background
    RGB(100, 180, 255),  // Light blue (more vibrant)
    RGB(0, 100, 200),    // Medium blue (higher contrast)
    RGB(0, 40, 120)      // Dark blue (higher contrast)
};

// Food palette - High contrast red theme
const UWORD food_palette[] = {
    RGB(248, 248, 248),  // Almost white background
    RGB(255, 100, 100),  // Bright red (more vibrant)
    RGB(220, 0, 0),      // Medium red (higher contrast)
    RGB(120, 0, 0)       // Dark red (higher contrast)
};

// Border palette - High contrast purple theme
const UWORD border_palette[] = {
    RGB(248, 248, 248),  // Almost white background
    RGB(200, 120, 255),  // Light purple (more vibrant)
    RGB(140, 60, 200),   // Medium purple (higher contrast)
    RGB(80, 0, 120)      // Dark purple (higher contrast)
};

// Text palette - High contrast dark theme
const UWORD text_palette[] = {
    RGB(248, 248, 248),  // Almost white background
    RGB(60, 60, 60),     // Light gray
    RGB(30, 30, 30),     // Dark gray
    RGB(0, 0, 0)         // Black (maximum contrast)
};

// Game over palette - High contrast orange/red theme
const UWORD gameover_palette[] = {
    RGB(248, 248, 248),  // Almost white background
    RGB(255, 160, 0),    // Bright orange (more vibrant)
    RGB(255, 80, 0),     // Orange-red (higher contrast)
    RGB(200, 0, 0)       // Dark red (higher contrast)
};

// Initialize all color palettes
void init_palettes(void) {
    // Check if running on Game Boy Color
    if (_cpu == CGB_TYPE) {
        // Set background palettes
        set_bkg_palette(BG_PALETTE_GREEN, 1, &bg_palette_green[0]);
        set_bkg_palette(SNAKE_PALETTE, 1, &snake_palette[0]);
        set_bkg_palette(FOOD_PALETTE, 1, &food_palette[0]);
        set_bkg_palette(BORDER_PALETTE, 1, &border_palette[0]);
        set_bkg_palette(TEXT_PALETTE, 1, &text_palette[0]);
        set_bkg_palette(GAMEOVER_PALETTE, 1, &gameover_palette[0]);
        
        // Set sprite palettes (using the same colors)
        set_sprite_palette(0, 1, &snake_palette[0]);
        set_sprite_palette(1, 1, &food_palette[0]);
    }
} 