#ifndef SNAKE_H
#define SNAKE_H

#include <gb/gb.h>

typedef enum {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN
} Direction;

// Initialize the snake game
void init_snake(void);

// Change the snake's direction
void change_direction(Direction new_dir);

// Update the snake's position
void update_snake(void);

// Check if the game is over
uint8_t is_game_over(void);

// Get the current score
uint8_t get_score(void);

// Get the high score
uint8_t get_high_score(void);

// Update the high score if the current score is higher
void update_high_score(void);

#endif
