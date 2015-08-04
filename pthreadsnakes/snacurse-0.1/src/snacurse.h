#ifndef MAIN_H
#define MAIN_H

#define BODY_HEIGHT 80
#define BODY_WIDTH 200
#define MAX_DISTANCE 280
#define MAX_ART 7 //物品种类数目
#define SNAKE_HEAD_DISTANCE 2
#define ART_NUM 7 //出现在屏幕上的物品数目
#define SNAKE_NUM 3 // 我们认为死蛇也是蛇的一种,所以实际的蛇数目是SNAKE_NUM-1
#define DEAD_SNAKE_ID 0
#define USER_SNAKE_ID 1
#define AI_SNAKE_ID 2
typedef struct point{
     int x,y;
     struct point *next;
}Point;

typedef struct{
  int color;
  int wall;
}State;

typedef struct{
  int color_id;
  int gpa;
  int speed;
}Art_mode;

typedef struct{
  int art_id;
  Point pos;
  Art_mode art_mode;
}Article;

typedef enum {
  POINT_FREE = 0,
  POINT_CONNECTABLE = 1,
  POINT_CHECKED = 2,
  POINT_WALL = 3
}Point_enum;

typedef enum {
  SNAKE_UP = 0x0010,
  SNAKE_DOWN = 0x0100,
  SNAKE_LEFT = 0x1000,
  SNAKE_RIGHT = 0x0001,
  SNAKE_STAY = 0
}Snake_dir;

typedef enum {
  SNAKE_LONGER = 16001,
  SNAKE_DEAD = 16002,
  SNAKE_CHANGE_ART = 16003,
  SNAKE_SAME = 16004,
  SNAKE_ALIVE = 16005
}Snake_state;

typedef struct{
  int id;
  int gpa;
  int len;
  int direction;
  unsigned int speed;
  Point* head;
  Point* tail; 
  Point art;
}Snake;

#endif
