#ifndef AI_H
#define AI_H

#include <time.h>
#include <math.h>
#include "draw.h"
#include "snake.h"
#include <string.h>
typedef struct w{
  int wall_id;
  int wall_num;
  char wall_name[30];
  int wall_x[2000];
  int wall_y[2000];
  struct w *next;
} Wall_xy;


void article_place(int art_id);
long article_move(int art_id);
Snake_dir choose_direction(Snake *snake);
void article_effect(Snake *snake,int wall_num);
void printwall();	//询问是否存储地图

#endif
