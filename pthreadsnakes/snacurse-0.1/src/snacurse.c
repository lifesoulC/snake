#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdbool.h>
#include "snake.h"
#include "draw.h"
#include "article.h"
#include "mymenu.h"
#include "wrapper.h"
#include "ai.h"

#define NUM_THREADS 4

sem_t wall_mutex;
sem_t dir_mutex;
sem_t stdscr_mutex;
sem_t art_mutex;

int remain_space;
State state[BODY_HEIGHT][BODY_WIDTH];
Snake *user_snake;
Snake *ai_snake;
int body_x,body_y;
int current_x, current_y;
int menu_snake, menu_art,menu_wall;
Wall_xy *wallxy,*p;

void *runner_keyboard();
void *runner_user_automove();
void *runner_ai_automove();
void *runner_artmove(void *);
Snake_dir read_direction();
void mode1();
void mode2();
void mode3();
extern Snake_dir choose_direction(Snake *snake);
extern Snake_dir choose_direction2(Snake *snake);

void start_game(){
     int i;

     srand((unsigned)time(NULL));
     menu();		//进入菜单
     keypad(stdscr, TRUE);
     init_screen();	//初始化屏幕
     init_state();	//初始化state数组
     draw_wall(menu_wall);	//初始化墙
     //初始化物品
     for(i = 0; i<menu_art; i++)
	  article_place(i);
}

void about_us(){
     printf("Snacurse v0.2\n");
     printf("Author: \n");
     printf("       Darcy Shen  <sadhen@mail.ustc.edu.cn>\n");
     printf("       Luo Na     	<ln931022@mail.ustc.edu.cn>\n");
     printf("       Xin Meng   	<xinmm@mail.ustc.edu.cn>\n");
     printf("       Zhu Xueqing	<xqzhu@mail.ustc.edu.cn>\n");
     printf("Current Maintainer: Darcy Shen\n");
     printf("Website: http://gitcafe.com/sadhen/playground\n");
}

void end_game(){

     endwin();
     about_us();
}

void check_snacurse_dir (){
     char *envhome;
     char buf[100];
     int code;

     buf[0] = '\0';
     envhome = getenv("HOME");
     strcat(buf,"test -d ");
     strcat(buf,envhome);
     strcat(buf,"/.snacurse");
     if(system(buf)){
	  buf[0] = '\0';
	  strcat(buf,"mkdir ");
	  strcat(buf,envhome);
	  strcat(buf,"/.snacurse");
	  code = system(buf);
	  if(code != 0){
	       fprintf(stderr,"%s failed",buf);
	       exit(0);
	  }
     }
}

int main(){

     //显示信息
     check_snacurse_dir();
     splash();
     Sem_init(&stdscr_mutex, 0, 1);
     Sem_init(&wall_mutex, 0, 1);
     Sem_init(&art_mutex, 0, 1);
     while(1){
	  start_game();
	  init_border();
	  switch(menu_snake){
	  case 1:
	       mode1();
	       break;
	  case 2:
	       mode2();
	       break;
	  case 3:
	       mode3();
	       break;
	  default:
	       break;
	  }
	  if(menu_wall == 1)
	       printwall();
	  if(menu_snake != 3)
	       print_score();
	  clear();
	  if(ai_snake)
	       free_snake(ai_snake);
	  if(user_snake)
	  free_snake(user_snake);
     }
     return 0;
}

void *runner_keyboard(){
     int dir;

     while(!user_snake)
	  ;
     while(1){
	  switch(menu_snake){
	  case 1:
	  case 2:
	       if(user_snake->id){
		    dir = read_direction();
		    snake_turn(user_snake, dir);	
		    usleep(user_snake->speed);
		    continue;
	       }
	       else{
		    while(getch() - 'q')
			 ;
		    ai_snake->id = 0;
		    break;
	       }
	       break;
	  case 3:
	       Sem_wait(&stdscr_mutex);
	       attron(COLOR_PAIR(8));
	       mvprintw(body_x,body_y/2 - 4,"Press q to stop");
	       attroff(COLOR_PAIR(8));
	       Sem_post(&stdscr_mutex);
	       while(getch() - 'q')
		    ;
	       ai_snake->id = 0;
	       user_snake->id = 0;
	       Sem_wait(&stdscr_mutex);
	       attron(COLOR_PAIR(8));
	       mvprintw(body_x,body_y/2 - 4,"Press q to quit");
	       attroff(COLOR_PAIR(8));
	       Sem_post(&stdscr_mutex);
	       while(getch() - 'q')
		    ;
	       break;
	  default:
	       break;
	  }
	  break;
     }	
     pthread_exit(0);
}

void *runner_user_automove(){
     int dir;
     
     Pthread_detach(Pthread_self());
     while(!user_snake)
	  ;
     while(user_snake->id != DEAD_SNAKE_ID && snake_automove(user_snake)){
	  if(menu_snake == 3){
	       dir = choose_direction2(user_snake);
	       snake_turn(user_snake, dir);
	  }

	  Sem_wait(&stdscr_mutex);
	  attron(COLOR_PAIR(8));
	  mvprintw(body_x,0,"USER:%4d %4d",user_snake->gpa,user_snake->speed/100);
	  attroff(COLOR_PAIR(8));
	  Sem_post(&stdscr_mutex);

	  usleep(user_snake->speed);
     }
     if(menu_snake != 3){
	  Sem_wait(&stdscr_mutex);
	  attron(COLOR_PAIR(8));
	  mvprintw(body_x,body_y/2 - 4,"Press q to quit");
	  attroff(COLOR_PAIR(8));
	  Sem_post(&stdscr_mutex);
     }
     pthread_exit(0);
}

void *runner_ai_automove(){
     int dir2 = SNAKE_STAY;

     Pthread_detach(Pthread_self());
     while(!user_snake)
	  ;
     while(ai_snake->id != DEAD_SNAKE_ID && snake_automove(ai_snake)){
	  dir2 = choose_direction(ai_snake);
	  snake_turn(ai_snake,dir2);
	  Sem_wait(&stdscr_mutex);
	  attron(COLOR_PAIR(8));
	  mvprintw(body_x,body_y-13,"AI:%4d %4d",ai_snake->gpa,ai_snake->speed/100);
	  attroff(COLOR_PAIR(8));
	  Sem_post(&stdscr_mutex);
	  usleep(ai_snake->speed);
     }
     pthread_exit(0);
}

void *runner_artmove(void *param){
     int a_speed;
     int num;
     int *num_to_free;

     num_to_free = (int*) param;
     num = *num_to_free;
     free(num_to_free);
     while(!user_snake)
	  ;
     while(user_snake->id || ai_snake->id){
	  a_speed = article_move(num);
	  usleep(a_speed);
     }
     pthread_exit(0);
}


/*
 * receive direction code from the keyboard
 */
Snake_dir read_direction(){
     int ch;

     ch = getch();
     if(user_snake->id == 0)
	  return SNAKE_STAY;
     switch(ch){
     case KEY_LEFT:
	  return SNAKE_LEFT;
     case KEY_RIGHT:
	  return SNAKE_RIGHT;
     case KEY_UP:
	  return SNAKE_UP;
     case KEY_DOWN:
	  return SNAKE_DOWN;
     default:
	  return SNAKE_STAY;
     }
}

void mode1(){
     pthread_t tid[NUM_THREADS];
     pthread_t art_tid[ART_NUM];
     pthread_attr_t attr;
     int i;
     int *num;

     user_snake = snake_create(USER_SNAKE_ID,0,0,SNAKE_DOWN,150000);
     ai_snake = snake_create(AI_SNAKE_ID,body_x-1, body_y-1,SNAKE_UP,240000);
     put_point(*user_snake->head,'@');
     put_point(*ai_snake->head,'X');
     //线程相关
     pthread_attr_init(&attr);
     pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
     pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
     Pthread_create(&tid[1],&attr,runner_keyboard,NULL);
     Pthread_create(&tid[2],&attr,runner_user_automove,NULL);
     Pthread_create(&tid[3],&attr,runner_ai_automove,NULL);
     for(i = 0; i < menu_art; i++){
	  num = (int *)Malloc(sizeof(int));
	  *num = i;
	  Pthread_create(&art_tid[i],NULL,runner_artmove,num);
	  }
     Pthread_join(tid[1], NULL);
     for(i = 0; i< menu_art; i++)
	  Pthread_join(art_tid[i],NULL);
}

void mode2(){
     pthread_t tid[NUM_THREADS];
     pthread_t art_tid[ART_NUM];
     pthread_attr_t attr;
     int i;
     int *num;

     user_snake = snake_create(USER_SNAKE_ID,0,0,SNAKE_DOWN,100000);
     ai_snake = snake_create(AI_SNAKE_ID,1, 1,SNAKE_UP,100000);
     ai_snake->id = 0;
     state[1][1].wall = 0;
     put_point(*user_snake->head,'@');
     //线程相关
     pthread_attr_init(&attr);
     pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
     pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
     Pthread_create(&tid[1],&attr,runner_keyboard,NULL);
     Pthread_create(&tid[2],&attr,runner_user_automove,NULL);
     for(i = 0; i < menu_art; i++){
	  num = (int *)Malloc(sizeof(int));
	  *num = i;
	  Pthread_create(&art_tid[i],NULL,runner_artmove,num);
     }
     Pthread_join(tid[1], NULL);
     for(i = 0; i< menu_art; i++)
	  Pthread_join(art_tid[i],NULL);
}


void mode3(){
     pthread_t tid[NUM_THREADS];
     pthread_t art_tid[ART_NUM];
     pthread_attr_t attr;
     int i;
     int *num;

     user_snake = snake_create(USER_SNAKE_ID,0,0,SNAKE_DOWN,50000);
     ai_snake = snake_create(AI_SNAKE_ID,body_x-1, body_y-1,SNAKE_UP,50000);
     put_point(*user_snake->head,'#');
     put_point(*ai_snake->head,'X');
     //线程相关
     pthread_attr_init(&attr);
     pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
     pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
     Pthread_create(&tid[1],&attr,runner_keyboard,NULL);
     Pthread_create(&tid[2],&attr,runner_user_automove,NULL);
     Pthread_create(&tid[3],&attr,runner_ai_automove,NULL);	
     for(i = 0; i < menu_art; i++){
	  num = (int *)Malloc(sizeof(int));
	  *num = i;
	  Pthread_create(&art_tid[i],NULL,runner_artmove,num);
     }

     Pthread_join(tid[1], NULL);
     for(i = 0; i< menu_art; i++)
	  Pthread_join(art_tid[i],NULL);
}
