#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include "snacurse.h"
#include "draw.h"
#include "article.h"
#include "wrapper.h"

//functions related to Snake

bool snake_automove(Snake *snake);
void snake_turn(Snake *snake,Snake_dir new_direction);
Snake *snake_create(int snake_id, int x, int y, Snake_dir direction, unsigned int speed);
Snake_state snake_check_next(Snake *snake,Snake_dir direction);
Snake_state snake_check(Snake *snake, Point snake_head);
void free_snake(Snake *snake);
Snake_state head_crash(Point point);
int get_wall_value(int x, int y);
void put_wall_value(int x, int y, int value);
bool valid_direction(Snake_dir dir);
bool is_opposite(Snake_dir dir1,Snake_dir dir2);
bool snake_next(Point *point,Snake_dir direction);

#endif
