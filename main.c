#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include "list_ptr.h"
#define WAIT_TIME 50
#define GROWTH 10
#define MAX_FOOD 10
#define NO_DEATH TRUE

WINDOW *gWGame, *gWStats;
typedef enum {UP, DOWN, LEFT, RIGHT} t_dir;


void initGame () {
	srand(time(NULL));
	setlocale(LC_ALL, "");
	initscr();
	timeout(WAIT_TIME); // if no no input then dont block
	//cbreak();
	//start_color();
	keypad(stdscr, TRUE); // use of the arrow keys
	noecho(); // no echo (obvious :p)
	curs_set(0); // no cursor
	
	gWGame= subwin(stdscr, LINES - 3, COLS, 0, 0);
	gWStats= subwin(stdscr, 3, COLS, LINES - 3, 0);
	
	box(gWGame, ACS_VLINE, ACS_HLINE);
	box(gWStats, ACS_VLINE, ACS_HLINE);

	mvwprintw(gWGame, 0, 1, "Snake IT!");
	mvwprintw(gWStats, 0, 1, "Stats");

	wrefresh(gWGame);
	wrefresh(gWStats);
	
	
}

void displaySnake(int isAlive, int eraseMode, int currDir) {
	t_pos pos;
	
	listPtr_move2head();
	while (!listPtr_isOut ()) {
		listPtr_readData (&pos);
		if (eraseMode) mvwprintw(gWGame, pos.line, pos.col, " ");
		else {
			switch (pos.body) {
				case HORIZONTAL:   mvwprintw(gWGame, pos.line, pos.col, "═"); break;
				case VERTICAL:     mvwprintw(gWGame, pos.line, pos.col, "║"); break;
				case TOP_LEFT:     mvwprintw(gWGame, pos.line, pos.col, "╔"); break;
				case TOP_RIGHT:    mvwprintw(gWGame, pos.line, pos.col, "╗"); break;
				case BOTTOM_LEFT:  mvwprintw(gWGame, pos.line, pos.col, "╚"); break;
				case BOTTOM_RIGHT: mvwprintw(gWGame, pos.line, pos.col, "╝"); break;
			}
		}
		listPtr_next ();
	}
	
	listPtr_move2head();
	listPtr_readData (&pos);
	if (isAlive) {
		if (eraseMode) mvwprintw(gWGame, pos.line, pos.col, " ");
		else {
			switch (currDir) {
				case UP: mvwprintw(gWGame, pos.line, pos.col, "^"); break;
				case DOWN: mvwprintw(gWGame, pos.line, pos.col, "v"); break;
				case RIGHT: mvwprintw(gWGame, pos.line, pos.col, ">"); break;
				case LEFT: mvwprintw(gWGame, pos.line, pos.col, "<"); break;
			}
		}
	}
	else
		mvwprintw(gWGame, pos.line, pos.col, "!");
}

void displayStats(int foodEat, int length, int foodQtt) {
	mvwprintw(gWStats, 1, 1, "foodEat: %d, length: %d, foodQtt: %d", foodEat, length, foodQtt);
	wrefresh(gWStats);
}

void displayFood(t_pos * foods, int foodQtt) {
	int i;
	for (i = 0; i < foodQtt; i++) {
		mvwprintw(gWGame, foods[i].line, foods[i].col, "%c", '%');
	}
	
}

void randomFood(t_pos * foods, int i) {
	foods[i].line = rand()%(LINES - 3 - 2) + 1;
	foods[i].col  = rand()%(COLS - 2) + 1;
}

int eat(t_pos head, t_pos * foods, int foodQtt, int * foodEat, int * growth) {
	int i, eatSometh = FALSE;
	for (i = 0; i < foodQtt; i++) {
		if (head.line == foods[i].line && head.col == foods[i].col) {
			randomFood(foods, i);
			eatSometh = TRUE;
			(*foodEat)++;
			(*growth)+=GROWTH;
		}
	}
	return eatSometh;
}

int main(void) {
	int key = -1;
	int foodEat = 0;
	int length = 1;
	int growth = 0;
	int foodQtt = 1;
	t_dir currDir = RIGHT, prevDir;
	int continueGame = TRUE;
	t_pos head = {1, 1, HORIZONTAL}, prev;
	t_pos foods[MAX_FOOD];
	
	initGame();
	randomFood(foods, 0);
	
	listPtr_init ();
	listPtr_appendHead (head);
	
	while (continueGame) {
		key = getch();
		prevDir = currDir;
		switch (key) {
			case 'q':
				continueGame = FALSE; break;
				
			case KEY_UP:
				if (currDir != DOWN) currDir = UP;
				break;
				
			case KEY_DOWN:
				if (currDir != UP) currDir = DOWN;
				break;
				
			case KEY_LEFT:
				if (currDir != RIGHT) currDir = LEFT;
				break;
				
			case KEY_RIGHT:
				if (currDir != LEFT) currDir = RIGHT;
				break;
				
			default:
				break;
		}
		
		listPtr_move2head();
		listPtr_readData (&head);
		prev = head;
		
		switch (currDir) {
			case UP: head.line --; break;
			case DOWN: head.line ++; break;
			case RIGHT: head.col ++; break;
			case LEFT: head.col --; break;
		}
		
		if (prevDir == currDir) {
			if (currDir == UP || currDir == DOWN) prev.body = VERTICAL;
			else prev.body = HORIZONTAL;
		}
		else {
			if ((prevDir == UP && currDir == RIGHT) || (prevDir == LEFT && currDir == DOWN))
				prev.body = TOP_LEFT;
				
			else if ((prevDir == UP && currDir == LEFT) || (prevDir == RIGHT && currDir == DOWN))
				prev.body = TOP_RIGHT;
				
			else if ((prevDir == DOWN && currDir == RIGHT) || (prevDir == LEFT && currDir == UP))
				prev.body = BOTTOM_LEFT;
				
			else
				prev.body = BOTTOM_RIGHT;
		}
		listPtr_move2head();
		listPtr_removeElt ();
		listPtr_appendHead (prev);
		
		if (head.line >= LINES - 3 - 1) head.line = 1;
		else if (head.line <= 0) head.line = LINES - 3 - 2;
		
		if (head.col >= COLS - 1) head.col = 1;
		else if (head.col <= 0) head.col = COLS - 2;
		
		if (listPtr_isInList(head) && NO_DEATH == FALSE) continueGame = FALSE;
		else {
			listPtr_appendHead (head);
			if (eat(head, foods, foodQtt, &foodEat, &growth) && foodQtt + 1 < MAX_FOOD) {
				randomFood(foods, foodQtt);
				foodQtt++;
			}
			
			if (growth == 0) {
				listPtr_move2end ();
				listPtr_removeElt ();
			}
			else {
				growth--;
				length++;
			}
			
			displayFood(foods, foodQtt);
			
			displaySnake(continueGame, FALSE, currDir);
			wrefresh(gWGame);
			displaySnake(continueGame, TRUE, currDir);
			
			displayStats(foodEat, length, foodQtt);
		}
	}
	
	listPtr_appendHead (head);
	displaySnake(continueGame, FALSE, currDir);
	wrefresh(gWGame);
	
	listPtr_removeList ();
	
	displayStats(foodEat, length, foodQtt);
	
	timeout(-1);
	getch();
	
	endwin();
	delwin(gWGame);
	delwin(gWStats);

	return 0;
}