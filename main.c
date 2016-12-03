#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "list_ptr.h"
#define WAIT_TIME 50

WINDOW *gWGame, *gWStats;
typedef enum {UP, DOWN, LEFT, RIGHT} t_dir;


void initGame () {
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

void displaySnake(int isAlive, int eraseMode) {
	t_pos pos;
	
	listPtr_move2head();
	while (!listPtr_isOut ()) {
		listPtr_readData (&pos);
		mvwprintw(gWGame, pos.line, pos.col, "%c", eraseMode ? ' ' : '#');
		listPtr_next ();
	}
	
	listPtr_move2head();
	listPtr_readData (&pos);
	if (isAlive)
		mvwprintw(gWGame, pos.line, pos.col, "%c", eraseMode ? ' ' : '@');
	else
		mvwprintw(gWGame, pos.line, pos.col, "!");
}

void displayStats(int score) {
	mvwprintw(gWStats, 1, 1, "score: %d", score);
	wrefresh(gWStats);
}

void displayFood(t_pos food) {
	mvwprintw(gWGame, food.line, food.col, "F");
}

void randomFood(t_pos * food) {
	food -> line = rand()%(LINES - 3 - 1) + 1;
	food -> col = rand()%(COLS - 1) + 1;
}

int eat(t_pos head, t_pos food) {
	return (head.line == food.line && head.col == food.col);
}

int main(void) {
	int key = -1;
	int score = 0;
	t_dir currDir = RIGHT;
	int continueGame = TRUE;
	t_pos head = {1, 1};
	t_pos food;
	randomFood(&food);
	
	initGame();
	listPtr_init ();
	listPtr_appendHead (head);
	
	while (continueGame) {
		key = getch();
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
		
		switch (currDir) {
			case UP: head.line --; break;
			case DOWN: head.line ++; break;
			case RIGHT: head.col ++; break;
			case LEFT: head.col --; break;
		}
		
		if (head.line >= LINES - 3 - 1) head.line = 1;
		else if (head.line <= 0) head.line = LINES - 3 - 2;
		
		if (head.col >= COLS - 1) head.col = 1;
		else if (head.col <= 0) head.col = COLS - 2;
		
		if (listPtr_isInList(head)) continueGame = FALSE;
		else {
			listPtr_appendHead (head);
			if (eat(head, food)) {
				score++;
				randomFood(&food);
			}
			else {
				listPtr_move2end ();
				listPtr_removeElt ();
			}
			
			displayFood(food);
			
			displaySnake(TRUE, FALSE);
			wrefresh(gWGame);
			displaySnake(TRUE, TRUE);
			
			displayStats(score);
		}
		
		// mvwprintw(gWGame, head.line, head.col, "#");
		// wrefresh(gWGame);
		// mvwprintw(gWGame, head.line, head.col, " ");
	}
	
	listPtr_appendHead (head);
	displaySnake(FALSE, FALSE);
	wrefresh(gWGame);
	
	listPtr_removeList ();
	
	displayStats(score);
	
	timeout(-1);
	getch();
	
	endwin();
	delwin(gWGame);
	delwin(gWStats);

	return 0;
}
