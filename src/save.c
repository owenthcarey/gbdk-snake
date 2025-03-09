#include "save.h"

// Save data structure in SRAM
// This must be compiled with -Wf-ba0 flag to place it in SRAM bank 0
__at (0xA000) uint16_t save_check_value;  // Magic number to verify save data exists
__at (0xA002) uint8_t saved_high_score;   // The saved high score

// Magic number to identify valid save data
#define SAVE_CHECK_VALUE 0x1234

// Check if save data exists
uint8_t has_save_data(void) {
    uint8_t result;
    
    // Enable RAM access
    ENABLE_RAM;
    
    // Check if the magic number is present
    result = (save_check_value == SAVE_CHECK_VALUE);
    
    // Disable RAM access
    DISABLE_RAM;
    
    return result;
}

// Load the high score from SRAM
uint8_t load_high_score(void) {
    uint8_t score;
    
    // Enable RAM access
    ENABLE_RAM;
    
    // Check if save data exists
    if (save_check_value == SAVE_CHECK_VALUE) {
        // Load the high score
        score = saved_high_score;
    } else {
        // No save data, return 0
        score = 0;
    }
    
    // Disable RAM access
    DISABLE_RAM;
    
    return score;
}

// Save the high score to SRAM
void save_high_score(uint8_t score) {
    // Enable RAM access
    ENABLE_RAM;
    
    // Save the high score
    saved_high_score = score;
    
    // Set the check value to indicate valid save data
    save_check_value = SAVE_CHECK_VALUE;
    
    // Disable RAM access
    DISABLE_RAM;
} 