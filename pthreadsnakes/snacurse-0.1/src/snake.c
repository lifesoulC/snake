#include "snake.h"

//用于多线程中共享数据的保护
extern sem_t wall_mutex;
extern sem_t stdscr_mutex;
extern int menu_art;
extern int menu_snake;
extern int body_x,body_y;
extern int current_x,current_y;
extern int dead;
extern Snake *user_snake,*ai_snake;
extern State state[BODY_HEIGHT][BODY_WIDTH];
extern int remain_space;


//判断两个方向是否相反
bool is_opposite(Snake_dir dir1,Snake_dir dir2){
     int dir;
  
     dir = dir1 | dir2;
     return (dir == 0x1001)||(dir == 0x0110);
}

int get_wall_value(int x, int y){
     int wall_num;

     Sem_wait(&wall_mutex);
     wall_num = state[x][y].wall;	
     Sem_post(&wall_mutex);
     
     return wall_num;
}

void put_wall_value(int x, int y, int value){
     
     Sem_wait(&wall_mutex);
     state[x][y].wall = value;
     Sem_post(&wall_mutex);
}

void free_snake(Snake *snake){
     Point *temp;
     Point *temp_next;

     temp = snake->tail;
     while(temp){
	  temp_next = temp->next;
	  free(temp);
	  temp = temp_next;
     }
     free(snake);
}

//从尾至头更新蛇身体上的颜色
void snake_tail2head(Snake *snake){
     int current_color,pre_color;
     Point* dot;

     dot = snake->tail;
     pre_color = state[dot->x][dot->y].color;
     current_color = pre_color;
     while(dot->next != NULL){
	  dot = dot->next;
	  current_color = state[dot->x][dot->y].color;
	  state[dot->x][dot->y].color = pre_color;
	  pre_color = current_color;
	  if(snake->id == USER_SNAKE_ID)
	       put_point(*dot,'#');
	  else
	       put_point(*dot,'X');
     }
}


void update_head(Snake *snake, int x,int y){
     Point *new_head;
     
     //mark the state 
     put_wall_value(x, y, 1);

     //generate the new head
     new_head = (Point *)malloc(sizeof(Point));
     new_head->x = x;
     new_head->y = y;
     new_head->next = NULL;
     snake->head->next = new_head;
     snake->head = new_head;
}

/*
 * 蛇的长度没有变，则从蛇尾更新到当前的蛇头-1的位置，
 * 把原来的蛇尾用空字符去掉，并将上一次头的颜色记录下来
 * 变长的蛇和正常的蛇的区别在于有没有除去尾巴
 * 蛇尾的显示已经在上面一段代码中用空字符去除
 * 这里是把蛇尾的在地图上的标记去掉，并把snake中的蛇尾往前移一步
 */

void update_tail(Snake *snake){
     Point *temp;

     temp = snake->tail;

     snake_tail2head(snake);
     put_blank(temp->x, temp->y);
     put_wall_value(temp->x, temp->y, 0);
     snake->tail = snake->tail->next;
     free(temp);
}

Snake *snake_create(int id, int x, int y, Snake_dir dir, unsigned int speed){
     Snake *new_snake;

     new_snake = (Snake*) Malloc(sizeof(Snake));
     new_snake->id = id;
     new_snake->len = 1;
     new_snake->head = (Point *)Malloc(sizeof(Point));
     new_snake->tail = (Point *)Malloc(sizeof(Point));
     new_snake->head->x = x;
     new_snake->head->y = y;
     new_snake->head->next = NULL;
     new_snake->tail = new_snake->head;
     new_snake->speed = speed;
     new_snake->gpa = 0;
     new_snake->direction = dir;
     state[x][y].wall = 1;
     
     return new_snake;
}

//撞墙或者另一条蛇而死
Snake_state head_crash(Point point){
     int num;
     
     num = get_wall_value(point.x, point.y);

     if (num != 1)
	  return SNAKE_ALIVE;

     return SNAKE_DEAD;
}

Snake_state snake_check(Snake *snake, Point snake_head){
     int num;

     num = get_wall_value(snake_head.x,snake_head.y);
     if (head_crash(snake_head) == SNAKE_DEAD)
	  return SNAKE_DEAD;

     if (num){
	  article_effect(snake,num);		//根据蛇吃到的物品，相应地改变蛇的一些参数
	  return SNAKE_LONGER;
     }

     return SNAKE_SAME;
}

bool snake_next(Point *point,Snake_dir direction){

     //将临时的蛇头照着方向向前走一步,这里只是算出一个坐标，并没有在state中标记

     switch(direction){
     case SNAKE_UP:
	  point->x = (point->x - 1 + body_x) % body_x;
	  return true;
     case SNAKE_DOWN:
	  point->x = (point->x + 1 + body_x) % body_x;
	  return true;
     case SNAKE_LEFT:
	  point->y = (point->y - 1 + body_y) % body_y;
	  return true;
     case SNAKE_RIGHT:
	  point->y = (point->y + 1 + body_y) % body_y;
	  return true;
     default:
	  return false;
     }
}


/*
 * 给外部调用的蛇的主函数
 * 如果蛇死了，则返回false
 * 否则返回true
 */
bool snake_automove(Snake *snake){
     int longer = SNAKE_SAME;
     Point snake_head,snake_pre_head;

     snake_head = *snake->head;
     snake_pre_head = *snake->head;			//save the current snake head
     snake_next(&snake_head,snake->direction);
     longer = snake_check(snake,snake_head);			//检测临时的蛇头有没有死亡，或者吃到东西，把return code 返回到longer中
     update_head(snake,snake_head.x,snake_head.y);		//把临时的蛇头赋值给真实的蛇头

     switch(longer){
     case SNAKE_DEAD:
	  snake->id = DEAD_SNAKE_ID;
	  return false;
     case SNAKE_LONGER:		   //if the snake is longer,just cover the head with the very character
	  if(snake->id == USER_SNAKE_ID)	
	       put_point(snake_pre_head,'#');
	  else 
	       put_point(snake_pre_head,'X');
	  break;
     case SNAKE_SAME:
	  update_tail(snake);
	  break;
     default:
	  break;
     }

     // print the head of the snake
     if(snake->id == USER_SNAKE_ID)
	  put_point(snake_head,'@');
     else
	  put_point(snake_head,'O');

     return true;
}



bool valid_direction(Snake_dir dir){
//     return true;
     if(dir!=SNAKE_UP && dir!=SNAKE_DOWN && dir!=SNAKE_LEFT && dir!=SNAKE_RIGHT && dir!=SNAKE_STAY)
	  return false;
     return true;
}

/*
 * 给snake指定新方向new_direction
 */
void snake_turn(Snake *snake,Snake_dir new_direction){
     int old_direction;

     old_direction = snake->direction;

     if(new_direction != SNAKE_STAY && !is_opposite(new_direction,old_direction))
	  snake->direction = new_direction;
}
