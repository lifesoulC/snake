#ifndef DRAW_H
#define DRAW_H
#include <stdlib.h>
#include "snacurse.h"
#include "wrapper.h"
#include "snake.h"
#include "article.h"

typedef struct pointrecord{
     int gpa;
     int length;
     int remain_space;
} PointRecord;

void init_screen();
void init_border();
void put_blank(int x,int y);
void put_sign(int x,int y);
void put_cover(int x,int y);
void put_snake(Snake *snake, char symbol);
void put_article(Article article);
void put_point(Point point,char symbol);
void get_screen();
void mywall();
void draw_wall(int id);
void print_score();
#endif

