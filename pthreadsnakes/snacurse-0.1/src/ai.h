#ifndef AI_H
#define AI_H
#include <stdio.h>
#include <stdbool.h>
#include "snacurse.h"
#include <stdlib.h>
#include "wrapper.h"
#include "snake.h"

Snake_dir choose_direction(Snake *snake);
Snake_dir choose_direction2(Snake *snake);
#endif
