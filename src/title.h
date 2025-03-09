#ifndef TITLE_H
#define TITLE_H

#include <gb/gb.h>

// Initialize the title screen
void init_title_screen(void);

// Update the title screen (returns 1 when player presses start to begin game)
uint8_t update_title_screen(void);

#endif 