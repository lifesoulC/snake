#ifndef MENU_H
#define MENU_H
#include "snacurse.h"
#include "draw.h"
#include<string.h>
#include <menu.h>
#include <unistd.h>

void splash();
void menu();
void init_state();
void start_game();
void end_game();
void print_in_middle(WINDOW *win,int startx,int starty,int width,char *string,chtype color);
#endif
