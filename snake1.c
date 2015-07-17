#include<curses.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

    void   init_keyboard(void);
	void   close_keyboard(void);
    int      kbhit(void);
	int     readch(void);


void p_move();
void food();
void judge_head();
void gameover();

 static struct termios initial_settings, new_settings;
     static int peek_character = -1;
	     void init_keyboard()
	    {
		        tcgetattr(0,&initial_settings);
				        new_settings = initial_settings;
						        new_settings.c_lflag &= ~ICANON;
								        new_settings.c_lflag &= ~ECHO;
										        new_settings.c_lflag &= ~ISIG;
												        new_settings.c_cc[VMIN] = 1;
														        new_settings.c_cc[VTIME] = 0;
																        tcsetattr(0, TCSANOW, &new_settings);
																		    }
    void close_keyboard()
	    {
		        tcsetattr(0, TCSANOW, &initial_settings);
				    }
    int kbhit()
	    {
		    unsigned char ch;
		    int nread;
		        if (peek_character != -1) return 1;
				        new_settings.c_cc[VMIN]=0;
						        tcsetattr(0, TCSANOW, &new_settings);
								        nread = read(0,&ch,1);
										        new_settings.c_cc[VMIN]=1;
												        tcsetattr(0, TCSANOW, &new_settings);
														        if(nread == 1) 
																	        {
																			            peek_character = ch;
																			           return 1;
																			       }
		        return 0;
		    }
    int readch()
	    {
		    char ch;
		        if(peek_character != -1) 
					        {
							            ch = peek_character;
							            peek_character = -1;
							            return ch;
							       }
		       read(0,&ch,1);
			          return ch;
					      }
struct snake
{
	short x;
	short y;
	struct snake *next;
}*head = NULL, *scan = NULL, *pf = NULL, *pb = NULL,
 *head1 = NULL,*scan1 = NULL, *pf1 = NULL, *pb1 = NULL;

char direction = 'd';
short snake_xy[1024];
short food_x, food_y, tail_x, tail_y,tail1_x,tail1_y;
short length = 4;
short socre,socre1;

void init()
{
	initscr();
//	border(0,0,0,0,0,0,0,0);
	box(stdscr,0,0);
	noecho();
	keypad(stdscr,TRUE);
	curs_set(0);
//	mvwhline(stdscr,2,10,'=',20);
//
	refresh();

}

void initsnake()
{
	short i;
	short a;
	for(i = 1; i <= length; i++)
	{
      pb = (struct snake*)malloc(sizeof(struct snake));
	  pb -> y = 17;
	  if(i == 1)
	  {
		  head = scan = pf = pb;
		  pb-> x = 10;
		  mvaddch(pb -> y, pb-> x,'#');
		  refresh();
	  }
	  else
	  {
		  pb-> x = pf->x-1;
          mvaddch(pb->y,pb->x,'#');
		  refresh();
		  pf-> next = pb;
	  }
	  pf = pb;
	  pf->next = NULL;
	}
	//refresh();
}

void gameover()
{
	clear();
	mvprintw(LINES/2,COLS/2,"GAME OVER!!!");
	mvprintw(LINES/2+1,COLS/2,"NO.1 snake socre:%d",socre);
	mvprintw(LINES/2+2,COLS/2,"NO.2 snake socre:%d",socre1);
    getch();
	endwin();
}
void movesnake()
{
    int a,b;
	char key_direction = 'd';
	while(!kbhit())
	{
        loop:
		mvaddch(food_y,food_x,'@');
		refresh();
		switch(direction)
		{
			case 'w':mvaddch(scan->y-1,scan->x,'#');refresh(); p_move();head->y = head->y-1;break;
			case 'a':mvaddch(scan->y,scan->x-1,'#');refresh(); p_move();head->x = head->x-1;break;
			case 's':mvaddch(scan->y+1,scan->x,'#');refresh(); p_move();head->y = head->y+1;break;
			case 'd':mvaddch(scan->y,scan->x+1,'#');refresh(); p_move();head->x = head->x+1;break;
		}
		scan = head;
		judge_head();
		sleep(1);
	}
	    scan = head;
	    a = getch();
		switch(a)
		{
			case KEY_UP:key_direction = 'w'; if(direction == 's') key_direction = 0; break;
			case KEY_DOWN:key_direction = 's'; if(direction == 'w') key_direction = 0; break;
			case KEY_LEFT:key_direction = 'a'; if(direction == 'd') key_direction = 0; break;
			case KEY_RIGHT:key_direction = 'd'; if(direction == 'a') key_direction = 0; break;
		    default:goto loop;
		}
		if(key_direction)
		{
			direction = key_direction;
		}
		else
			goto loop;
		goto loop;


}

void p_move()
{
	short i = 0;
	while(scan->next != NULL)
		scan = scan-> next;
    mvaddch(scan->y, scan->x,' ');
    refresh();
	scan = head;
	while(scan->next != NULL)
	{
		snake_xy[i] = scan->x;
		i++;
		snake_xy[i] = scan->y;
		i++;
		scan = scan->next;
	}
	tail_x = scan->x;
	tail_y = scan->y;
	i = 0;
	scan = head->next;
	while(scan->next != NULL)
	{
		scan->x = snake_xy[i];
		i++;
		scan->y = snake_xy[i];
		i++;
		scan = scan ->next;
	}
	scan->x = snake_xy[i];
	i++;
	scan->y = snake_xy[i];
}

void food()
{
	food_x = rand()%COLS;
	food_y = rand()%LINES;
	mvaddch(food_y,food_x,'@');
	refresh();
	scan = head;
}

void judge_head()
{
	short i = 0;
	if((head->x == 0) || (head->x == COLS) || (head->y == 0) || (head->y == LINES))
//	gameover();
     printw("1\n");
	 refresh();
	while(scan->next != NULL)
	{
		if(head->y == snake_xy[++i])
			if(head-> y == snake_xy[++i])
			  // gameover();
			   printw("h2\n");
		    else
			   i++;
		else
			i+=2;
		scan = scan-> next;
	}
	if(head->x == snake_xy[i])
		if(head->y == snake_xy[++i])
		//	gameover();
			printw("god 3\n");
	if(head-> x == food_x && head->y == food_y)
	{
		socre+=100;
		pb = (struct snake*)malloc(sizeof(struct snake));
		pf->next = pb;
		pb->next = NULL;
		pf = pb;
		pb->x = tail_x;
		pb->y = tail_y;
		mvaddch(pb->y,pb->x,'#');
		refresh();
		food();
	}
}
int main()
{
	init();

    initsnake();
	food();
	movesnake();
	getch();
	endwin();
}
