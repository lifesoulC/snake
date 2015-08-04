#include "draw.h"

extern int remain_space;
extern State state[BODY_HEIGHT][BODY_WIDTH];
extern sem_t stdscr_mutex;
extern int current_x;
extern int current_y;
extern int body_x;
extern int body_y;
extern Snake *user_snake;
static void init_colors();


void get_screen(){
  
     getmaxyx(stdscr,current_x,current_y);
     body_x = 23;
     body_y = 79;
     remain_space = body_x * body_y;
}

void init_border(){
     int i;
     
     Sem_wait(&stdscr_mutex);
     attron(COLOR_PAIR(8));
     for(i=0;i<body_y;i++)
	  mvaddch(body_x,i,' ');
     for(i=0;i<body_x+1;i++)
	  mvaddch(i,body_y,' ');
     attroff(COLOR_PAIR(8));
     Sem_post(&stdscr_mutex);
}

//初始化屏幕
void init_screen(){

     initscr();
     init_colors();
     get_screen();
     init_border();
     keypad(stdscr,TRUE);
     curs_set(0);
     noecho();
}

void init_colors(){

     Start_color();
     /*启动 color 机制 */
     init_pair(0,COLOR_WHITE, COLOR_BLACK);
     init_pair(1,COLOR_RED, COLOR_BLACK);
     init_pair(2,COLOR_GREEN,COLOR_BLACK);
     init_pair(3,COLOR_YELLOW,COLOR_BLACK);
     init_pair(4,COLOR_BLUE,COLOR_BLACK);
     init_pair(5,COLOR_CYAN,COLOR_BLACK);
     init_pair(6,COLOR_WHITE,COLOR_WHITE);
     init_pair(7,COLOR_BLACK,COLOR_BLACK);
     init_pair(8,COLOR_BLACK,COLOR_WHITE);
}

void put_blank(int x, int y){

     Sem_wait(&stdscr_mutex);
     attron(COLOR_PAIR(7));
     mvaddch(x, y,' ');
     attroff(COLOR_PAIR(7));
     refresh();
     Sem_post(&stdscr_mutex);
}

void put_point(Point point,char symbol){
     int color_pair;

     color_pair = state[point.x][point.y].color;
     Sem_wait(&stdscr_mutex);
     attron(COLOR_PAIR(color_pair));
     mvaddch(point.x, point.y, symbol);
     attroff(COLOR_PAIR(color_pair));
     refresh();
     Sem_post(&stdscr_mutex);
}

void put_article(Article article){

     Sem_wait(&stdscr_mutex);
     state[article.pos.x][article.pos.y].color = article.art_mode.color_id;
     attron(COLOR_PAIR(article.art_mode.color_id));
     mvaddch(article.pos.x,article.pos.y,'$');
     attroff(COLOR_PAIR(article.art_mode.color_id));
     refresh();   
     Sem_post(&stdscr_mutex);
}


void put_sign(int x,int y){

     Sem_wait(&stdscr_mutex);
     if(state[x][y].wall == 0){
	  attron(COLOR_PAIR(6));
	  mvaddch(x,y,' ');
	  attroff(COLOR_PAIR(6));
     }
     else if(state[x][y].wall == 1){
	  attron(COLOR_PAIR(8));
	  mvaddch(x,y,'H');
	  attroff(COLOR_PAIR(8));
     }
     refresh();
     Sem_post(&stdscr_mutex);

}

void put_cover(int x,int y){
     Sem_wait(&stdscr_mutex);
     if(state[x][y].wall == 1){
	  attron(COLOR_PAIR(0));
	  mvaddch(x,y,'H');
	  attroff(COLOR_PAIR(0));
     }
     else if(state[x][y].wall == 0){
	  attron(COLOR_PAIR(7));
	  mvaddch(x,y,' ');
	  attroff(COLOR_PAIR(7));
     }
     refresh();
     Sem_post(&stdscr_mutex);
}

void print_score(){
     FILE *storage;
     PointRecord PR[14];
     PointRecord UserPoint;
     int i,n,k,base_x;
     char *envhome;
     char fstore[100];

     envhome = getenv("HOME");
     strcpy(fstore,envhome);
     strcat(fstore,"/.snacurse/record");

     getmaxyx(stdscr,current_x,current_y);	//读取当前屏幕的大小
     base_x = current_x / 2 -10;		//取竖直方向上的基础值，使score list竖直方向上居中
     UserPoint.gpa = user_snake->gpa;
     UserPoint.length = user_snake->len;
     UserPoint.remain_space = remain_space;

     clear();

     attron(COLOR_PAIR(1));
     mvprintw(base_x,current_y/2-9,"Hight score list:\n");
     mvprintw(base_x+1,current_y/2-40,"|\tRank\t|\tGPA\t|\tLength\t|\tSpace\t|\n");
     attroff(COLOR_PAIR(1));

     if ((storage=fopen(fstore,"r")) != NULL){
	  n=fread(PR,sizeof(PointRecord),14,storage);
	  fclose(storage);
     } 
     else 
	  n = 0;		//n为文件中储存的分数的个数
     
     storage=fopen(fstore,"w");

     for (i=0,k=0; i<n; i++){
	  if (k==0 && PR[i].gpa < UserPoint.gpa) { 
	       fwrite(&UserPoint,sizeof(PointRecord),1,storage);
	       attron(COLOR_PAIR(2));
	       mvprintw(base_x+i+2,current_y/2-40,"|\t%d\t|\t%d\t|\t%d\t|\t%d\t|",
			i+1, UserPoint.gpa, UserPoint.length, UserPoint.remain_space);
	       k = 1;
	       attroff(COLOR_PAIR(2));
	       
	       attron(COLOR_PAIR(3));
	       printw(" * ");
	       attroff(COLOR_PAIR(3));

	       attron(COLOR_PAIR(2));
	       if (i==0)
		    printw("New best!\n");
	       else 
		    printw("Your score.\n");
	       attroff(COLOR_PAIR(2));

	  }
	  fwrite(&(PR[i]), sizeof(PointRecord), 1, storage);

	  attron(COLOR_PAIR(3));
	  mvprintw(base_x+i+k+2, current_y/2-40, "|\t%d\t|\t%d\t|\t%d\t|\t%d\t|\n",
		   i+k+1, PR[i].gpa, PR[i].length, PR[i].remain_space);
	  attroff(COLOR_PAIR(3));
     }
    
     if (k==0){		//表明你的成绩没有超过任何现有的成绩
	  fwrite(&UserPoint,sizeof(PointRecord),1,storage);
	  attron(COLOR_PAIR(2));
	  mvprintw(base_x+i+2,current_y/2-40,"|\t%d\t|\t%d\t|\t%d\t|\t%d\t|",
		   n+1,UserPoint.gpa,UserPoint.length,UserPoint.remain_space);    
	  attroff(COLOR_PAIR(2));
	  attron(COLOR_PAIR(3));
	  printw(" * ");
	  attroff(COLOR_PAIR(3));
	  attron(COLOR_PAIR(2));
	  if (n==0)
	       printw("New best!\n");
	  else 
	       printw("Your score.\n");
	  attroff(COLOR_PAIR(2));
     }

     attron(A_REVERSE);
     mvprintw(current_x-1,0,"Press a to continue\n");
     attroff(A_REVERSE);

     while(getch() - 'a');
    
     fclose(storage);
}
