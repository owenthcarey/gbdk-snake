#ifndef SNAKE_H
#define SNAKE_H

#include <gb/gb.h>

typedef enum {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN
} Direction;

void init_snake(void);
void change_direction(Direction new_dir);
void update_snake(void);
uint8_t is_game_over(void);

#endif
