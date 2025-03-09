#ifndef PALETTES_H
#define PALETTES_H

#include <gb/gb.h>

// Retro color palettes for Game Boy Color
// Each palette has 4 colors: BG color, Color 1, Color 2, Color 3

// Background palette - Soft green retro theme
#define BG_PALETTE_GREEN 0
extern const UWORD bg_palette_green[];

// Snake palette - Teal/blue theme
#define SNAKE_PALETTE 1
extern const UWORD snake_palette[];

// Food palette - Red/pink theme
#define FOOD_PALETTE 2
extern const UWORD food_palette[];

// Border palette - Purple/lavender theme
#define BORDER_PALETTE 3
extern const UWORD border_palette[];

// Text palette - Dark blue theme
#define TEXT_PALETTE 4
extern const UWORD text_palette[];

// Game over palette - Red/orange theme
#define GAMEOVER_PALETTE 5
extern const UWORD gameover_palette[];

// Initialize all color palettes
void init_palettes(void);

#endif 