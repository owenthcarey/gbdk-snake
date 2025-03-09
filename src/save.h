#ifndef SAVE_H
#define SAVE_H

#include <gb/gb.h>

// Check if save data exists
uint8_t has_save_data(void);

// Load the high score from SRAM
uint8_t load_high_score(void);

// Save the high score to SRAM
void save_high_score(uint8_t score);

#endif 