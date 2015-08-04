#include "ai.h"

Snake_dir sample_choose_direction(Snake *snake);


Snake_dir choose_direction(Snake *snake){
     return sample_choose_direction(snake);
}

Snake_dir choose_direction2(Snake *snake){
     return sample_choose_direction(snake);
}

/*
 * Below is a example of ai snake.
 * As we know, we control the movement of the snake by controlling its head's direction
 * Your task is to provide a powful choose_direction function to avoid death and gain the best GPA
 */
extern int body_x,body_y;
extern sem_t stdscr_mutex;
extern Snake *user_snake,*ai_snake;
extern Article article[ART_NUM];
extern int remain_space;
extern int menu_art;
static int region_points_num = 0;
static int region_state[BODY_HEIGHT][BODY_WIDTH];
const int direction_array[4]={
     SNAKE_LEFT, SNAKE_RIGHT, SNAKE_UP, SNAKE_DOWN
};

/*
 * Below is related to point
 */
int min(int x, int y){
     if(x > y)
	  return y;
     return x;
}

int min3(int x, int y, int z){
     return min(x,min(y,z));
}

//简单粗暴的距离计算函数
int distance(Point A,Point B){
     int x,y;

     x = min3(abs(A.x - B.x),A.x-B.x+body_x,B.x-A.x+body_x);
     y = min3(abs(A.y - B.y),A.y-B.y+body_y,B.y-A.y+body_y);
     return x+y;
}

//用于预先判断蛇会不会死亡的辅助函数
void point_next(Point *point,Snake_dir direction){

     switch(direction){
     case SNAKE_UP:
	  point->x = (point->x - 1 + body_x) % body_x;
	  break;
     case SNAKE_DOWN:
	  point->x = (point->x + 1 + body_x) % body_x;
	  break;
     case SNAKE_LEFT:
	  point->y = (point->y - 1 + body_y) % body_y;
	  break;
     case SNAKE_RIGHT:
	  point->y = (point->y + 1 + body_y) % body_y;
	  break;
     case SNAKE_UP | SNAKE_LEFT:
	  point->x = (point->x - 1 + body_x) % body_x;
	  point->y = (point->y - 1 + body_y) % body_y;
	  break;
     case SNAKE_UP | SNAKE_RIGHT:
	  point->x = (point->x - 1 + body_x) % body_x;
	  point->y = (point->y + 1 + body_y) % body_y;
	  break;
     case SNAKE_DOWN | SNAKE_LEFT:
	  point->x = (point->x + 1 + body_x) % body_x;
	  point->y = (point->y - 1 + body_y) % body_y;
	  break;
     case SNAKE_DOWN | SNAKE_RIGHT:
	  point->x = (point->x + 1 + body_x) % body_x;
	  point->y = (point->y + 1 + body_y) % body_y;
	  break;
     default:
	  break;
     }
}

bool point_is_wall(Point point){
     int state_wall;

     state_wall = get_wall_value(point.x, point.y);
     
     if(state_wall == 1)
	  return true;
     return false;
}

bool out_of_range(Point point){
     Point *pp;
  
     pp = &point;
     if(pp->x < 0 || pp->x >= body_x || pp->y < 0 || pp->y >= body_y)
	  return true;
     return false;
}


int wall_around(Point point){
     Point temp;
     int cnt = 0;
     int i;
     int direction_array[8]={
	  SNAKE_LEFT, SNAKE_RIGHT, SNAKE_UP, SNAKE_DOWN,
	  SNAKE_LEFT | SNAKE_UP, SNAKE_LEFT | SNAKE_DOWN,
	  SNAKE_RIGHT | SNAKE_UP, SNAKE_RIGHT | SNAKE_DOWN
     };

     for(i=0; i < 8; i++){
	  temp = point;
	  point_next(&temp,direction_array[i]);
	  if(out_of_range(temp) || point_is_wall(temp)){
	       cnt++;
	  }
     }
     return cnt;
}

int wall_block(Point point){
     Point temp;
     int cnt = 0;
     int i;

     for (i=0; i < 4; i++){
	  temp = point;
	  point_next(&temp, direction_array[i]);
	  if(out_of_range(temp) || point_is_wall(temp))
	       cnt++;
     }
     return cnt;
}

void init_region_state(){
     Point pos;

     region_points_num = 0;
     for(pos.x=0; pos.x < body_x; pos.x++)
	  for(pos.y=0; pos.y < body_y; pos.y++)
	       region_state[pos.x][pos.y] = POINT_FREE;
}

void count_region_points(Point point){
     Point temp;
     int i;

     for(i=0; i < 4; i++){
	  temp = point;
	  point_next(&temp,direction_array[i]);
	  if(out_of_range(temp) || point_is_wall(temp) || region_state[temp.x][temp.y] == 1)
	       continue;
	  region_state[temp.x][temp.y] = 1;
	  region_points_num ++;
	  count_region_points(temp);
     }
}

int point_may_die(Point point){
     int wall_block_num;
     int wall_around_num;

     wall_around_num = wall_around(point);
     wall_block_num = wall_block(point);
     if(wall_block_num == 4)
	  return SNAKE_DEAD;
    
     if(wall_around_num >= 3){
	  init_region_state();
	  count_region_points(point);
	  if(region_points_num == remain_space)
	       return SNAKE_ALIVE;
     }

     return region_points_num;
}

Snake_dir get_order_dir(int num,int first,int second,int third,int fourth){
     switch(num){
     case 0:
	  return first;
     case 1:
	  return second;
     case 2:
	  return third;
     case 3:
	  return fourth;
     default:
	  fprintf(stderr,"Fatal error");
	  getch();
	  exit(1);
     }
}

/*
 * 根据给出的direction，检测下一个snake的下一步会不会死亡，如果死亡，则
 * 返回SNAKE_DEAD,否则，返回值不等于SNAKE_DEAD
 */
Snake_state snake_check_next(Snake *snake,Snake_dir dir){
     Point temp_point;

     temp_point = *snake->head;
     snake_next(&temp_point,dir);

     if(head_crash(temp_point) == SNAKE_DEAD)
	  return SNAKE_DEAD;

     if(snake->id == USER_SNAKE_ID && 
	ai_snake->id != DEAD_SNAKE_ID && 
	distance(temp_point,*ai_snake->head) < SNAKE_HEAD_DISTANCE)
	  return SNAKE_DEAD;

     if(snake->id != USER_SNAKE_ID && 
	user_snake->id != DEAD_SNAKE_ID && 
	distance(temp_point,*user_snake->head) < SNAKE_HEAD_DISTANCE)
	  return SNAKE_DEAD;

     return point_may_die(temp_point);
}

//根据给出顺序，来判断并选择蛇的方向
Snake_dir choose_order(Snake *snake,int first,int second,int third,int fourth){
     int check_result[4];
     int i,dir;
     int max,max_id,temp;


     dir = snake->direction;

     for(i=0; i < 4; i++){
	  if(is_opposite(dir,get_order_dir(i,first,second,third,fourth))){
	       check_result[i] = SNAKE_STAY;
	       continue;
	  }
	  check_result[i] = snake_check_next(snake,get_order_dir(i,first,second,third,fourth)); //预先测试该方向的可行性
     }

     max = -1;	//储存当前的最大空间
     max_id = -1;
     for(i=0; i < 4; i++){
	  temp = check_result[i];
	  if(temp == SNAKE_ALIVE) //如果能活下了，就选择该方向
	       return get_order_dir(i,first,second,third,fourth);
	  if(temp < SNAKE_LONGER && temp > max){	//如果不能活，就选择空间尽可能大的方向
	       max = temp;
	       max_id = i;
	  }
     }
     if(max != -1 ){
	  dir = get_order_dir(max_id,first,second,third,fourth);
	  if(!valid_direction(dir)){
	       printf("choose_order");
	       getch();
	  }
	  return dir;
     }

     return SNAKE_STAY;
}


//从序号为0~n-1的物品中找出与蛇距离最短的物品，并返回該物品的序号
int shortest_article(Point head,int n){
     int num;
     int dis;
     int temp,i;

     dis = MAX_DISTANCE;
     num = 0;
     for(i = 0; i<n; i++)
	  if(article[i].art_id)
	       if((temp = distance(head,article[i].pos)) < dis){
		    dis = temp;
		    num = i;
	       }
     return num;
}

//根据蛇的位置，物品的位置，选择合适的方向
Snake_dir sample_choose_direction(Snake *snake){
     int i,head_x,head_y,a_x,a_y;

     head_x = snake->head->x;
     head_y = snake->head->y;

     i = shortest_article(*snake->head, menu_art);

     snake->art = article[i].pos;
     a_x = snake->art.x;
     a_y = snake->art.y;

     /*
      * 下面这段又长又丑的代码是手动根据蛇所处的位置，为之选择方向
      */

     // LEFT
     if (a_x == head_x && a_y < head_y){
	  if(head_y - a_y > body_y/2){ // go through the wall
	       if(head_x > body_x/2)
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_UP,SNAKE_DOWN,SNAKE_LEFT);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_DOWN,SNAKE_UP,SNAKE_LEFT);
	  }
	  else{
	       if(head_x > body_x/2)
		    return choose_order(snake,SNAKE_LEFT,SNAKE_UP,SNAKE_DOWN,SNAKE_RIGHT);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_DOWN,SNAKE_UP,SNAKE_RIGHT);
	  }
     }
    
     //RIGHT
     if (a_x == head_x && a_y > head_y){
	  if(a_y - head_y > body_y/2){ //go through the wall
	       if(head_x > body_x/2)
		    return choose_order(snake,SNAKE_LEFT,SNAKE_UP,SNAKE_DOWN,SNAKE_RIGHT);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_DOWN,SNAKE_UP,SNAKE_RIGHT);
	  }
	  else{
	       if(head_x > body_x/2)
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_UP,SNAKE_DOWN,SNAKE_LEFT);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_DOWN,SNAKE_UP,SNAKE_LEFT);
	  }
     }

     //DOWN
     if (a_x > head_x && a_y == head_y){
	  if(a_x - head_x > body_x/2){ //go through the wall
	       if(head_y > body_y/2)
		    return choose_order(snake,SNAKE_UP,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_UP,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_DOWN);
	  }
	  else{
	       if(head_y > body_y/2)
		    return choose_order(snake,SNAKE_DOWN,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_DOWN,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_UP);
	  }
     }

     //UP
     if (a_x < head_x && a_y == head_y){
	  if(head_x - a_x > body_x/2){ //go through the wall
	       if(head_y > body_y/2)
		    return choose_order(snake,SNAKE_DOWN,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_DOWN,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_UP);
	  }
	  else{
	       if(head_y > body_y/2)
		    return choose_order(snake,SNAKE_UP,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_UP,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_DOWN);
	  }
     }

     // UP & LEFT
     if (a_x < head_x && a_y < head_y){
	  if(head_x - a_x > body_x/2 && head_y - a_y > body_y/2){
	       if(body_x + a_x - head_y > body_y + a_y - head_y)
		    return choose_order(snake,SNAKE_DOWN,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_DOWN,SNAKE_UP,SNAKE_LEFT);
	  }
	  else if(head_x - a_x > body_x/2 && head_y - a_y <= body_y/2){
	       if(body_x + a_x - head_x > head_y - a_y) 
		    return choose_order(snake,SNAKE_DOWN,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_DOWN,SNAKE_UP,SNAKE_RIGHT);
	  }
	  else if(head_x - a_x <= body_x/2 && head_y - a_y > body_y/2){
	       if(head_x - a_x > body_y + a_y - head_y)
		    return choose_order(snake,SNAKE_UP,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_UP,SNAKE_DOWN,SNAKE_LEFT);
	  }
	  else{
	       if(head_x - a_x > head_y - a_y)
		    return choose_order(snake,SNAKE_UP,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_UP,SNAKE_DOWN,SNAKE_RIGHT);
	  }
     }

     // UP & RIGHT
     if (a_x < head_x && a_y > head_y){
	  if(head_x - a_x > body_x/2 && a_y - head_y > body_y/2){
	       if(body_x + a_x - head_x > body_y + head_y - a_y) 
		    return choose_order(snake,SNAKE_DOWN,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_DOWN,SNAKE_UP,SNAKE_RIGHT);
	  }
	  else if(head_x - a_x > body_x/2 && a_y -head_y <= body_y/2){
	       if(body_x + a_x - head_x > a_y - head_y)
		    return choose_order(snake,SNAKE_DOWN,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_DOWN,SNAKE_UP,SNAKE_LEFT);
	  }
	  else if(head_x - a_x <= body_x/2 && a_y - head_y > body_y/2){
	       if(head_x - a_x > body_y + head_y - a_y)
		    return choose_order(snake,SNAKE_UP,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_UP,SNAKE_DOWN,SNAKE_RIGHT);
	  }
	  else{
	       if(head_x - a_x > a_y - head_y)
		    return choose_order(snake,SNAKE_UP,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_UP,SNAKE_DOWN,SNAKE_LEFT);
	  }
     }

     // DOWN & LEFT
     if (a_x > head_x && a_y < head_y){
	  if(a_x - head_x > body_x/2 && a_y - head_y > body_y/2){
	       if(body_x + head_x - a_x > body_y - a_y + head_y)
		    return choose_order(snake,SNAKE_UP,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_UP,SNAKE_DOWN,SNAKE_LEFT);
	  }
	  else if(a_x - head_x > body_x/2 && a_y - head_y <= body_y/2){
	       if(body_x + head_x - a_x > body_y + head_y - a_y)
		    return choose_order(snake,SNAKE_UP,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_UP,SNAKE_DOWN,SNAKE_RIGHT);
	  }
	  else if(a_x - head_x <= body_x/2 && a_y - head_y > body_y/2){
	       if(body_x - a_x + head_x > body_y - a_y + head_y)
		    return choose_order(snake,SNAKE_DOWN,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_DOWN,SNAKE_UP,SNAKE_LEFT);
	  }
	  else{
	       if(a_x - head_x > head_y - a_y) 
		    return choose_order(snake,SNAKE_DOWN,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_DOWN,SNAKE_UP,SNAKE_RIGHT);
	  }
     }
    
     // DOWN & RIGHT
     if (a_x > head_x && a_y > head_y){
	  if(a_x - head_x > body_x/2 && a_y - head_y > body_y/2){
	       if(body_x + head_x - a_x > body_y + head_y - a_y)
		    return choose_order(snake,SNAKE_UP,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_UP,SNAKE_DOWN,SNAKE_RIGHT);
	  }
	  else if(a_x - head_x > body_x/2 && a_y - head_y <= body_y/2){
	       if(body_x + head_x - a_x > a_y - head_y)
		    return choose_order(snake,SNAKE_UP,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_DOWN);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_UP,SNAKE_DOWN,SNAKE_LEFT);
	  }
	  else if(a_x - head_x <= body_x/2 && a_y - head_y > body_y/2){
	       if(a_x - head_x > body_y + head_y - a_y) 
		    return choose_order(snake,SNAKE_DOWN,SNAKE_LEFT,SNAKE_RIGHT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_LEFT,SNAKE_DOWN,SNAKE_UP,SNAKE_RIGHT);
	  }
	  else{
	       if(a_x - head_x > a_y - head_y)
		    return choose_order(snake,SNAKE_DOWN,SNAKE_RIGHT,SNAKE_LEFT,SNAKE_UP);
	       else
		    return choose_order(snake,SNAKE_RIGHT,SNAKE_DOWN,SNAKE_UP,SNAKE_LEFT);
	  }
     }
     return choose_order(snake,SNAKE_UP,SNAKE_DOWN,SNAKE_UP,SNAKE_LEFT);
}
