#include "mymenu.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define CDRAL 5
char* choices[]={
     "Start Game",
     "Model Settings",
     "Wall Settings",
     "Artical Settings",
     " ",
     (char*)NULL,
};
extern int remain_space;
extern sem_t dir_mutex;
extern sem_t stdscr_mutex;
extern sem_t wall_mutex;
extern Snake *user_snake;
extern Snake *ai_snake;
extern State state[BODY_HEIGHT][BODY_WIDTH];
extern int menu_snake,menu_art,menu_wall;
extern Wall_xy *wallxy,*p;
extern FILE *wall;
extern int body_x, body_y;
static int cho[5]={1,0,0,1,1};//the one to remember the choices we have made

void menu_modelpart(int* cho);
void menu_wallpart(int* cho);
void menu_articlepart(int* cho);

void splash(){
     int a,b,d,e,i,j;
     int max_x;
     int max_y;
     int menu_sta[BODY_HEIGHT][BODY_WIDTH];

     initscr();
     noecho();
     start_color();
     curs_set(0);
     init_pair(2,COLOR_RED,COLOR_BLACK);
     init_pair(1,COLOR_GREEN,COLOR_BLACK);
     init_pair(3,COLOR_YELLOW,COLOR_BLACK);
     getmaxyx(stdscr,max_y,max_x);
     a=max_x;
     b=max_y;
     clear();
     refresh();
     e=a;
     d=b/2;
     
     while(b>d){
	  for(i=0;i<max_x;i++){
	       move(max_y-b,i);
	       if(menu_sta[max_y-b][i]==0){
		    addch('#');
		    menu_sta[max_y-b][i]=1;
	       }
	       move(b-1,max_x-1-i);
	       if(menu_sta[b-1][max_x-1-i]==0){
		    addch('#');
		    menu_sta[b-1][max_x-1-i]=1;
		    refresh();
		    usleep(1000);
	       }
	  }
	  for(j=max_y;j>0;j--){
	       move(j,max_x-e);
	       if(menu_sta[j][max_x-e]==0){
		    addch('#');
		    menu_sta[j][max_x-e]=1;
	       }
	       move(max_y-1-j,e-1);
	       if(menu_sta[max_y-1-j][e-1]==0){
		    addch('#');
		    menu_sta[max_y-1-j][e-1]=1;
		    refresh();
		    usleep(1000);
	       }
	  }
	  b=b-1;
	  e=e-1;
     }
     move(max_y/2-1,max_x/2-5);
     attron(COLOR_PAIR(2) | A_BLINK);
     printw("SNACURSE");
     attroff(COLOR_PAIR(2) | A_BLINK);
     usleep(1000000);

     //make some simple flash
     for(i=0;i<5;i++){
	  attron(COLOR_PAIR(1));
	  move(i,max_x/2-6);
	  addch('#');
	  move(max_y-1-i,max_x/2+4);
	  addch('#');
	  attroff(COLOR_PAIR(1));
	  refresh();
	  usleep(10000);
     }
     for(i=0;i<max_y-5;i++){
	  attron(COLOR_PAIR(1));
	  move(i+5,max_x/2-6);
	  addch('#');
	  move(max_y-6-i,max_x/2+4);
	  addch('#');
	  attroff(COLOR_PAIR(1));
	  move(i,max_x/2-6);
	  addch('#');
	  move(max_y-1-i,max_x/2+4);
	  addch('#');
	  refresh();
	  usleep(10000);
     }
     for(i=0;i<5;i++){
	  move(max_y-5+i,max_x/2-6);
	  addch('#');
	  move(4-i,max_x/2+4);
	  addch('#');
	  refresh();
	  usleep(10000);
     }
     for(i=0;i<10;i++){
	  move(max_y/2-2,i);
	  attron(COLOR_PAIR(1));
	  addch('#');
	  move(max_y/2,max_x-1-i);
	  addch('#');
	  refresh();
	  usleep(10000);
     }
     for(i=0;i<max_x-10;i++){
	  attron(COLOR_PAIR(1));
	  move(max_y/2-2,i+10);
	  addch('#');
	  move(max_y/2,max_x-10-i);
	  addch('#');
	  attroff(COLOR_PAIR(1));
	  move(max_y/2-2,i);
	  addch('#');
	  move(max_y/2,max_x-1-i);
	  addch('#');
	  refresh();
	  usleep(10000);
     }
     for(i=0;i<10;i++){
	  move(max_y/2-2,max_x-10+i);
	  addch('#');
	  move(max_y/2,9-i);
	  addch('#');
	  refresh();
	  usleep(10000);
     }
     move(max_y/2+5,max_x/2-12);
     attron(COLOR_PAIR(1));
     printw("press SPACE to continue");
     attroff(COLOR_PAIR(1));
     while(getch() != 32)
	  ;
     clear();
     curs_set(0);
     endwin();
}

void menu(){
     ITEM **my_items;
     int c;
     MENU *my_menu;
     WINDOW *my_menu_win;
     int n_choices;
     int i;
     int max_x,max_y;

     /*
      * menu_name specification:
      * wall	0	1	2	3
      * 	没有墙  墙1	墙2	编辑墙
      *	snake   1	2	3
      *		对战	单打	观战
      *	art  	表示物品数目
      */
     
     cho[0]=1;//to remember the item you choose in the main menu

     while(cho[0]!=0&&cho[0]!=4){
	  initscr();
	  start_color();
	  cbreak();
	  noecho();
	  keypad(stdscr,TRUE);
	  curs_set(0);
	  init_pair(1,COLOR_RED,COLOR_BLACK);
	  init_pair(4,COLOR_BLUE,COLOR_BLACK);
	  init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
	  getmaxyx(stdscr,max_y,max_x);
	  n_choices=ARRAY_SIZE(choices);
	  my_items=(ITEM**)calloc(n_choices,sizeof(ITEM*));
	  for(i=0;i<n_choices;i++){
	       my_items[i]=new_item(choices[i],choices[4]);
	  }
	  my_menu=new_menu((ITEM**)my_items);

	  my_menu_win=newwin(15,40,max_y/2-3,max_x/2-10);
	  keypad(my_menu_win,TRUE);

	  set_menu_win(my_menu,my_menu_win);
	  set_menu_sub(my_menu,derwin(my_menu_win,6,38,10,1));

	  set_menu_mark(my_menu,"=> ");
	  attron(A_REVERSE);
	  mvprintw(LINES-1,0,"Enter to choose,ESC to exit");
	  attroff(A_REVERSE);
	  attron(COLOR_PAIR(5));
	  mvprintw(max_y/2-6,max_x/2-5,"Snacurse");
	  attroff(COLOR_PAIR(5));
	  for(i=0;i<30;i++){
	       attron(COLOR_PAIR(5));
	       mvprintw(max_y/2-5,max_x/2-15+i,"_");
	       attroff(COLOR_PAIR(5));
	  }
	  refresh();
	  set_menu_fore(my_menu,COLOR_PAIR(1));
	  set_menu_back(my_menu,COLOR_PAIR(4));

	  post_menu(my_menu);
	  wrefresh(my_menu_win);
	  cho[0]=0;
	  while((c=wgetch(my_menu_win))!=10){ // 10 stands for Enter
	       switch(c){
	       case KEY_DOWN:
		    if(cho[0]<n_choices-3){
			 menu_driver(my_menu,REQ_DOWN_ITEM);
			 cho[0]++;
		    }
		    break;
	       case KEY_UP:
		    if(cho[0]>0){
			 menu_driver(my_menu,REQ_UP_ITEM);
			 cho[0]--;
		    }
		    break;
	       case 27: //27 stands for ESC
		    end_game();
		    exit(0);
		    break;
	       }//we make the signal cho[0] the one exact the number we want;
	       refresh();
	       wrefresh(my_menu_win);
	  }//it is effident that the data can be brought outside
	  //now we have the cho[0] to tell us what you have choose
	  if(cho[0]==1){
	       menu_modelpart(cho);
	  }
	  if(cho[0]==2){
	       menu_wallpart(cho);
	  }
	  if(cho[0]==3){
	       menu_articlepart(cho);
	  }
	  unpost_menu(my_menu);
	  free_menu(my_menu);
	  for(i=0;i<n_choices;i++){
	       free_item(my_items[i]);
	  }
	  clear();
	  endwin();
     }
     menu_wall=cho[2];
     menu_snake=cho[1]+1;
     menu_art=cho[3];
     //the end of the main menu

     //set the afterwards datas for the main menu and to start the game 
     //we need some signals to mark the choices you have made
     //we can use a pile of numbers to memory the choices you have made

}
char* choices1[]={
     "MODLE 1 :USER VS AUTO",
     "MODLE 2 :USER ONLY",
     "MODLE 3 :AUTO VS AUTO",
     " ",
     (char*)NULL,
};


void menu_modelpart(int* cho){
     ITEM **my_items;
     int c;
     MENU *my_menu;
     WINDOW *my_menu_win;
     int n_choices;
     int i;
     int max_x,max_y;


     initscr();
     start_color();
     clear();
     cbreak();
     noecho();
     keypad(stdscr,TRUE);
     curs_set(0);
     init_pair(1,COLOR_RED,COLOR_BLACK);
     init_pair(4,COLOR_BLUE,COLOR_BLACK);
     init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
     getmaxyx(stdscr,max_y,max_x);

     n_choices=ARRAY_SIZE(choices1);
     my_items=(ITEM**)calloc(n_choices,sizeof(ITEM*));
     for(i=0;i<n_choices-1;i++){
	  my_items[i]=new_item(choices1[i],choices1[3]);
     }
     my_menu=new_menu((ITEM**)my_items);

     my_menu_win=newwin(15,40,max_y/2-3,max_x/2-10);
     keypad(my_menu_win,TRUE);

     set_menu_win(my_menu,my_menu_win);
     set_menu_sub(my_menu,derwin(my_menu_win,6,38,10,1));

     set_menu_mark(my_menu,"=> ");
     attron(A_REVERSE);
     mvprintw(LINES-1,0,"Press Enter to ensure");
     attroff(A_REVERSE);
     attron(COLOR_PAIR(5));
     mvprintw(max_y/2-6,max_x/2-6,"Model Settings");
     attroff(COLOR_PAIR(5));
     for(i=0;i<30;i++){
	  attron(COLOR_PAIR(5));
	  mvprintw(max_y/2-5,max_x/2-15+i,"_");
	  attroff(COLOR_PAIR(5));
     }
     refresh();
     set_menu_fore(my_menu,COLOR_PAIR(1));
     set_menu_back(my_menu,COLOR_PAIR(4));

     post_menu(my_menu);
     wrefresh(my_menu_win);
     cho[1]=0;
     while((c=wgetch(my_menu_win))!=10){ //10 stands for Enter
	  switch(c){
	  case KEY_DOWN:
	       if(cho[1]<n_choices-3){
		    menu_driver(my_menu,REQ_DOWN_ITEM);
		    cho[1]++;
	       }
	       break;
	  case KEY_UP:
	       if(cho[1]>0){
		    menu_driver(my_menu,REQ_UP_ITEM);
		    cho[1]--;
	       }
	       break;
	  }//we make the signal cho[0] the one exact the number we want;
	  refresh();
	  wrefresh(my_menu_win);
     }

     unpost_menu(my_menu);
     free_menu(my_menu);
     for(i=0;i<n_choices;i++){
	  free_item(my_items[i]);
     }
     clear();
     endwin();
}
char *choices2[]={
     "No wall",
     "Edit the wall",
     "Select the wall",
     " ",
     (char*)NULL,
};

void menu_wallpart(int* cho){
     ITEM **my_items;
     int c;
     MENU *my_menu;
     WINDOW *my_menu_win;
     int n_choices;
     int i;
     int max_x,max_y;


     initscr();
     start_color();
     clear();
     cbreak();
     noecho();
     keypad(stdscr,TRUE);
     curs_set(0);
     init_pair(1,COLOR_RED,COLOR_BLACK);
     init_pair(4,COLOR_BLUE,COLOR_BLACK);
     init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
     getmaxyx(stdscr,max_y,max_x);

     n_choices=ARRAY_SIZE(choices2);
     my_items=(ITEM**)calloc(n_choices,sizeof(ITEM*));
     for(i=0;i<n_choices-1;i++){
	  my_items[i]=new_item(choices2[i],choices2[3]);
     }
     my_menu=new_menu((ITEM**)my_items);

     my_menu_win=newwin(15,40,max_y/2-3,max_x/2-10);
     keypad(my_menu_win,TRUE);

     set_menu_win(my_menu,my_menu_win);
     set_menu_sub(my_menu,derwin(my_menu_win,6,38,10,1));

     set_menu_mark(my_menu,"=> ");
     attron(A_REVERSE);
     mvprintw(LINES-1,0,"Press Enter to ensure");
     attroff(A_REVERSE);
     attron(COLOR_PAIR(5));
     mvprintw(max_y/2-6,max_x/2-6,"Wall settings");
     attroff(COLOR_PAIR(5));
     for(i=0;i<30;i++){
	  attron(COLOR_PAIR(5));
	  mvprintw(max_y/2-5,max_x/2-15+i,"_");
	  attroff(COLOR_PAIR(5));
     }
     refresh();
     set_menu_fore(my_menu,COLOR_PAIR(1));
     set_menu_back(my_menu,COLOR_PAIR(4));

     post_menu(my_menu);
     wrefresh(my_menu_win);
     cho[2]=0;
     while((c=wgetch(my_menu_win))!=10){ //10 stands for enter
	  switch(c){
	  case KEY_DOWN:
	       if(cho[2]<n_choices-3){
		    menu_driver(my_menu,REQ_DOWN_ITEM);
		    cho[2]++;
	       }
	       break;
	  case KEY_UP:
	       if(cho[2]>0){
		    menu_driver(my_menu,REQ_UP_ITEM);
		    cho[2]--;
	       }
	       break;
	  }//we make the signal cho[0] the one exact the number we want;
	  refresh();
	  wrefresh(my_menu_win);
     }
     unpost_menu(my_menu);
     free_menu(my_menu);
     for(i=0;i<n_choices;i++){
	  free_item(my_items[i]);
     }
     clear();
     endwin();
}

char* choices3[]={
     "ONE",
     "TWO",
     "THREE",
     "FOUR",
     "FIVE",
     "SIX",
     "SEVEN",
     " ",
     (char*)NULL,
};
void  menu_articlepart(int* cho){
     ITEM **my_items;
     int c;
     MENU *my_menu;
     WINDOW *my_menu_win;
     int n_choices;
     int i;
     int max_x,max_y;


     initscr();
     start_color();
     clear();
     cbreak();
     noecho();
     keypad(stdscr,TRUE);
     curs_set(0);
     init_pair(1,COLOR_RED,COLOR_BLACK);
     init_pair(4,COLOR_BLUE,COLOR_BLACK);
     init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
     getmaxyx(stdscr,max_y,max_x);

     n_choices=ARRAY_SIZE(choices3);
     my_items=(ITEM**)calloc(n_choices,sizeof(ITEM*));
     for(i=0;i<n_choices-1;i++){
	  my_items[i]=new_item(choices3[i],choices3[7]);
     }
     my_menu=new_menu((ITEM**)my_items);

     my_menu_win=newwin(15,40,max_y/2-3,max_x/2-10);
     keypad(my_menu_win,TRUE);

     set_menu_win(my_menu,my_menu_win);
     set_menu_sub(my_menu,derwin(my_menu_win,6,38,10,1));

     set_menu_mark(my_menu,"=> ");
     attron(A_REVERSE);
     mvprintw(LINES-1,0,"Press Enter to ensure");
     attroff(A_REVERSE);
     attron(COLOR_PAIR(5));
     mvprintw(max_y/2-6,max_x/2-6,"Article number");
     attroff(COLOR_PAIR(5));
     for(i=0;i<30;i++){
	  attron(COLOR_PAIR(5));
	  mvprintw(max_y/2-5,max_x/2-15+i,"_");
	  attroff(COLOR_PAIR(5));
     }
     refresh();
     set_menu_fore(my_menu,COLOR_PAIR(1));
     set_menu_back(my_menu,COLOR_PAIR(4));

     post_menu(my_menu);
     wrefresh(my_menu_win);
     cho[3]=1;
     while((c=wgetch(my_menu_win)) != 10){ //10 stands for Enter
	  switch(c){
	  case KEY_DOWN:
	       if(cho[3]<n_choices-2){
		    menu_driver(my_menu,REQ_DOWN_ITEM);
		    cho[3]++;
	       }
	       break;
	  case KEY_UP:
	       if(cho[3]>0){
		    menu_driver(my_menu,REQ_UP_ITEM);
		    cho[3]--;
	       }
	       break;
	  }//we make the signal cho[0] the one exact the number we want;
	  refresh();
	  wrefresh(my_menu_win);
     }
     unpost_menu(my_menu);
     free_menu(my_menu);
     for(i=0;i<n_choices;i++){
	  free_item(my_items[i]);
     }
     clear();
     endwin();

}

void print_in_middle(WINDOW *win,int starty,int startx,int width,char *string,chtype color){
     int length,x,y;
     float temp;
     if(win==NULL)
	  win=stdscr;
     getyx(win,y,x);
     if(startx!=0){
	  x=startx;
     }
     if(starty!=0){
	  y=starty;
     }
     if(width==0){
	  width=80;
     }
     length=strlen(string);
     temp=(width-length)/2;
     x=startx+(int)temp;
     wattron(win,color);
     mvwprintw(win,y,x,"%s",string);
     wattroff(win,color);
     refresh();
}//it still needs some improvement

void init_state(){
     int i,j;

     for(i=0; i < BODY_HEIGHT; i++)
	  for(j=0; j < BODY_WIDTH; j++)
	       state[i][j].wall = 0;
}


