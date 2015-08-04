#include "article.h"

extern sem_t art_mutex;
extern Snake *ai_snake,*user_snake;
extern int remain_space;
extern int body_x,body_y;
extern int region_state[BODY_HEIGHT][BODY_WIDTH];
extern bool valid_direction(Snake_dir);
extern Wall_xy *wallxy,*p;
unsigned int seedp1 = 1;
unsigned int seedp2 = 2;
unsigned int seedp3 = 3;
unsigned int seedp4 = 4;
unsigned int seedp5 = 5;

static Art_mode art_mode[MAX_ART] = {
     {0, 0, 0},
     {0, 0, 0},
     {1,20,600000},
     {2,40,500000},
     {3,60,400000},
     {4,80,300000},
     {5,100,200000}
};
Article article[ART_NUM];
int art_state[ART_NUM][BODY_HEIGHT][BODY_WIDTH];
extern State state[BODY_HEIGHT][BODY_WIDTH];

/*
 *随机产生x,y,物品序列,并在屏幕上显示物品
 */

void article_place(int art_id){
     int num = 1,x,y;

     while(num != 0){
	  x = rand_r(&seedp3) % body_x;
	  y = rand_r(&seedp4) % body_y;

	  Sem_wait(&art_mutex);
	  article[art_id].pos.x = x;
	  article[art_id].pos.y = y;
	  Sem_post(&art_mutex);

	  num = get_wall_value(x, y);
     }
     num = rand_r(&seedp5)% 5 + 2;
     
     Sem_wait(&art_mutex);
     article[art_id].art_mode = art_mode[num];
     article[art_id].art_id = art_id +1; 
     put_article(article[art_id]);
     Sem_post(&art_mutex);

     put_wall_value(x, y, art_id + 2);
}

//随机移动序号为art_id的物品
long article_move(int art_id){
     int move_x = 1 ,move_y = 1;
     int new_x, new_y;
     int old_x, old_y;
     int state_wall;

     if(article[art_id].art_id == 0)
	  return 0;
     Sem_wait(&art_mutex);
     new_x = article[art_id].pos.x;
     new_y = article[art_id].pos.y;
     Sem_post(&art_mutex);
     
     state_wall = get_wall_value(new_x, new_y);

     while((new_x < 0 )|| (new_y < 0) || (state_wall != 0) || (new_x >= body_x) || (new_y >= body_y)){
	  move_x = rand_r(&seedp1)% 30;
	  move_y = rand_r(&seedp2)% 30;

	  Sem_wait(&art_mutex);
	  new_x = (article[art_id].pos.x + ((move_x+1)%3) - 1) % body_x;
	  new_y = (article[art_id].pos.y + ((move_y+1)%3) - 1) % body_y;
	  Sem_post(&art_mutex);

	  state_wall = get_wall_value(new_x, new_y);
     }
     
     put_wall_value(new_x, new_y, art_id + 2);
     Sem_wait(&art_mutex);
     old_x = article[art_id].pos.x;
     old_y = article[art_id].pos.y;
     Sem_post(&art_mutex);

     put_wall_value(old_x, old_y, 0);
     put_blank(old_x, old_y);

     Sem_wait(&art_mutex);
     article[art_id].pos.x = new_x;
     article[art_id].pos.y = new_y;
     put_article(article[art_id]);
     Sem_post(&art_mutex);

     return article[art_id].art_mode.speed;
}

void article_effect(Snake *snake,int wall_num){
     int art_id;

     if(wall_num >= 2){
    
	  art_id = wall_num -2;
	  snake->len ++;
	  remain_space --;

	  snake->gpa +=article[art_id].art_mode.gpa;
	  if(snake->speed > 0)
	       snake->speed -= article[art_id].art_mode.gpa*10;
	  if(snake->speed <= 5000)
	       snake->speed += 5000;

	  Sem_wait(&art_mutex);
	  article[art_id].art_id = 0;
	  Sem_post(&art_mutex);

	  article_place(art_id);
     }
}

/*
 * all below is related with the functions of the wall map
 * 1. provide a simple but functional editor to allow user to draw its own wall
 * 2. allow user to choose wall
 */

//初始化结构体指针
Wall_xy *pre_set(){  
     int i;
     Wall_xy *wall;

     wall = (Wall_xy*)malloc(sizeof(Wall_xy));
     wall->wall_id = 0;
     wall->wall_num = 0;
     wall->wall_name[0] = 'r';
     wall->wall_name[1] = '\0';
     wall->next = NULL;
     for(i = 0;i < 2000;i++){
	  wall->wall_x[i] = -1;
	  wall->wall_y[i] = -1;
     }
     return wall;
}

//从文件读进一个地图
Wall_xy *get_line(FILE *fp){
     int i;
     Wall_xy *wall;
     char c;

     if((c = getc(fp)) == '#')
	  return NULL;
     else if(c == '*'){
	  wall = (Wall_xy*)malloc(sizeof(Wall_xy));
	  fscanf(fp,"%d",&(wall->wall_id));
	  fscanf(fp,"%s",wall->wall_name);
	  fscanf(fp,"%d",&(wall->wall_num));  
	  for(i = 0;i < wall->wall_num;i++)
	       fscanf(fp,"%d %d",&(wall->wall_x[i]),&(wall->wall_y[i]));
	  while(getc(fp) != '\n')
	       ;
	  return wall;
     }
     else
	  return NULL;
}

//将地图信息读进链表
void getwallxy(){
     Wall_xy *wall,*temp; 
     FILE *fwall;					
     char *envhome;
     char fstore[100];

     envhome = getenv("HOME");
     strcpy(fstore,envhome);
     strcat(fstore,"/.snacurse/wallxy");
     wallxy = NULL;		

     if((fwall  = fopen(fstore,"r")) != NULL){

	  wall = (Wall_xy*)malloc(sizeof(Wall_xy));
	  while((temp = get_line(fwall))!=NULL){
	       if(wallxy == NULL){
		    wallxy = temp;
		    wall = temp;
	       }
	       else{
		    wall->next = temp;
		    wall = wall->next;
	       }
	  }
	  wall->next = NULL;
  
	  fclose(fwall);
     }
     return;
}

//将链表写进文件
void putwallxy(){
     Wall_xy *p0,*p1;
     int i = 0;
     FILE *wall;
     char *envhome;
     char fstore[100];

     envhome = getenv("HOME");
     strcpy(fstore,envhome);
     strcat(fstore,"/.snacurse/wallxy");

     if((wall = fopen(fstore,"w")) == NULL){
	  fprintf(stderr,"Cannot creat the wallxy!");
	  exit(1);
     }
     p1 = p0 = (Wall_xy *)malloc(sizeof(Wall_xy));
     p0 = wallxy;


     while(p0 != NULL){
       p1 = p0;
	 
	  if(p0->wall_id == 0){
	    p0 = p1->next;
	    free(p1);
	    continue;
	  }
	  fprintf(wall,"%c",'*');
	  fprintf(wall,"%d %s %d ",p0->wall_id,p0->wall_name,p0->wall_num);
	  for(i = 0;i < p0->wall_num;i++)
	       fprintf(wall,"%d %d ",p0->wall_x[i],p0->wall_y[i]);
	  fprintf(wall,"%c",'\n');
	  if(p0->next == NULL)
	       fprintf(wall,"%c",'#');
	  p0 = p1->next;
	  free(p1);
     }
     fclose(wall);
     return;
}

//地图编辑器
void mywall(){
  int key,i,j,k = 0;
  int now_x,now_y,top = 0;
  while(p->wall_x[k] != -1)
    k++;
  top = k;
     attron(COLOR_PAIR(8));
     mvprintw(body_x,0,"Esc -> Start Game, Space -> Put Wall, Backspace -> Delete Wall");
     attroff(COLOR_PAIR(8));
     now_x = body_x/2;
     now_y = body_y/2;
     put_sign(now_x,now_y);
     while((key = getch()) != 27){ //ESC = 27

	  switch(key){
	  case KEY_LEFT:
	       put_cover(now_x,now_y);
	       now_y = (now_y - 1 + body_y) % body_y;
	       put_sign(now_x,now_y);
	       break;
	  case KEY_RIGHT:
	       put_cover(now_x,now_y);
	       now_y = (now_y + 1) % body_y;
	       put_sign(now_x,now_y);
	       break;
	  case KEY_UP :
	       put_cover(now_x,now_y);
	       now_x = (now_x - 1 + body_x) % body_x;
	       put_sign(now_x,now_y);
	       break;
	  case KEY_DOWN :
	       put_cover(now_x,now_y);
	       now_x = (now_x + 1) % body_x;
	       put_sign(now_x,now_y);
	       break;
	  case ' ' ://space
	       if(state[now_x][now_y].wall == 0){
		    state[now_x][now_y].wall = 1;
		    p->wall_x[top] = now_x;
		    p->wall_y[top++] = now_y;
		    p->wall_num++;
	       }
	       put_cover(now_x,now_y);
	       if(now_y < body_y - 1)
		    put_sign(now_x,++now_y);
	       break;
	  case KEY_BACKSPACE://backspace
	       if(now_y-1<0)	//如果到了边界，就停止
		    break;
	       now_y--;		//向左走一格
	       if(state[now_x][now_y].wall == 1){
		    for(i = 0;i < p->wall_num;i++){
			 if(p->wall_x[i] == now_x && p->wall_y[i] == now_y){
			      for(j = i; j< p->wall_num; j++){
				   p->wall_x[j] = p->wall_x[j+1];
				   p->wall_y[j] = p->wall_y[j+1];
			      }
			      break;
			 }
		    }
		    p->wall_num--;
		    state[now_x][now_y].wall = 0;
		    top--;
	       }
	       put_cover(now_x,now_y+1);
	       put_sign(now_x,now_y);
	       break;
	  case KEY_DC: // Delete
	       for(i = 0;i < p->wall_num;i++){
		    if(p->wall_x[i] == now_x && p->wall_y[i] == now_y){
			 for(j = i; j< p->wall_num; j++){
			      p->wall_x[j] = p->wall_x[j+1];
			      p->wall_y[j] = p->wall_y[j+1];
			 }
			 break;
		    }
	       }
	       p->wall_num--;
	       state[now_x][now_y].wall = 0;
	       top--;
	       put_sign(now_x,now_y);
	       break;
	  default :
	       break;
	  }
     }
     put_cover(now_x,now_y);
}

void printwall(){   //是否存储地图的处理
     Wall_xy *p0;
     char command[30];
     int current_x,current_y,i;

     clear();
     init_border();
     attron(COLOR_PAIR(3));
     for(i = 0;i < body_x;i++)
       mvaddch(i,body_y/4 -1,'+');
     for(i = 0;i < body_x;i++)
       mvaddch(i,3*body_y/4 + 1,'+');
     attroff(COLOR_PAIR(3));
     mvprintw(0,body_y/4,"Do you want to save the wall map?[y/n]");
     getyx(stdscr,current_x,current_y);
     refresh();
     echo();
     curs_set(2);
     mvscanw(current_x,current_y,"%s",command);
     while(strcmp(command,"n") != 0){
	  if(strcmp(command,"y") == 0){
	       getyx(stdscr,current_x,current_y);
	       mvprintw(current_x,body_y/4,"Please input the name for it:");
	       refresh();
	       move(current_x+1,body_y/4);
	       mvscanw(current_x + 1,body_y/4,"%s",p->wall_name);
	       p0 = (Wall_xy*)malloc(sizeof(Wall_xy));
	       p0 = wallxy;
	       if(p0 != NULL){
		    while(p0->next != NULL){
			 p0 = p0->next;
		    }
		    p->wall_id = 1;
		    p0->next = p;
	       }
	       else if(p0 == NULL){
		    p->wall_id = 1;
		    wallxy = p;
	       }
	       putwallxy();
	       break;
	  }
	  else {
	       getyx(stdscr,current_x,current_y);
	       mvprintw(current_x,body_y/4,"Error input! Input again:");
	       refresh();
	       scanw("%s",command);
	  }
     }
}

void display_wall(Wall_xy *current){ //打印地图形状供用户选择
     int i;
     int x,y;

     //清屏
     clear();
     init_border();
     attron(COLOR_PAIR(8));
     mvprintw(body_x,0,"| Name: %6s | Next: Space | Start: Enter | Delete:d | Modify:m |",current->wall_name);
	  
     attroff(COLOR_PAIR(8));
     attron(COLOR_PAIR(0));
     for(i=0; i < current->wall_num; i++){
	  x = current->wall_x[i];
	  y = current->wall_y[i];
	  mvaddch(x,y,'H');
     }
     attroff(COLOR_PAIR(0));
     refresh();
}

void set_wall(Wall_xy *current){  //选定后的处理
     int i;
     int x,y;

     //清屏
     if(current == NULL)
	  return ;
     clear();
     init_border();
     for(i=0; i < current->wall_num; i++){
	  x = current->wall_x[i];
	  y = current->wall_y[i];
	  state[x][y].wall = 1;
	  put_cover(x,y);
	  refresh();
     }
}

Wall_xy *select_wall(){   //选则地图
  Wall_xy *temp,*p1;
     char ch;
     int i;

     temp = wallxy;
     while(temp){
       if(temp->wall_id == 0){
	 temp = temp->next;
	 if(temp == NULL)
	   temp = wallxy;
	 p1 = wallxy;
	 while(p1 != NULL && p1->wall_id == 0){
	   p1 = p1->next;
	   if(p1 == NULL){
	     clear();
	  
	     return NULL;
	   }

	 }
	 continue;
       }
       display_wall(temp);
       refresh();
       while(1){
	 ch = getch();
	 if(ch == 32) // SPACE
	   break;
	 else if(ch == 10) // ENTER
	   return temp;
	 else if(ch == 'd'){ //bakespace
	   temp->wall_id = 0;
	   break;
	 }
	 else if(ch == 'm'){
	   set_wall(temp);
	   for(i = 0;i < temp->wall_num;i++){
	     p->wall_x[i] = temp->wall_x[i];
	     p->wall_y[i] = temp->wall_y[i];
	   }
	   temp->wall_id = 0;
	   p->wall_num = temp->wall_num;
	   p->wall_id = 1;
	   strcpy(p->wall_name,temp->wall_name);
	   mywall();
	   p1 = wallxy;
	   while(p1->next != NULL)
	     p1 = p1->next;
	   p1->next = p;
	   return p;
	 }
       }
       temp = temp->next;
       if(temp == NULL)
	       temp = wallxy;
     }
     clear();
     init_border();
     return NULL;
}

void draw_wall(int id){   //地图模式

     p = pre_set();
     getwallxy();
     switch(id){
     case 1:
	  mywall();
	  break;
     case 2:
	  set_wall(select_wall());
	  putwallxy();
	  break;
     default:
	  break;
     }
}
