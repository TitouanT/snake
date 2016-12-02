#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "list_ptr.h"
#define WAIT_TIME 50

WINDOW *gWGame, *gWStats;
typedef enum {UP, DOWN, LEFT, RIGHT} t_dir;


void initGame () {
	initscr();
	start_color();
	noecho();
	curs_set(0);
	
	gWGame= subwin(stdscr, LINES - 3, COLS, 0, 0);
	gWStats= subwin(stdscr, 3, COLS, LINES - 3, 0);
	
	box(gWGame, ACS_VLINE, ACS_HLINE);
	box(gWStats, ACS_VLINE, ACS_HLINE);

	mvwprintw(gWGame, 0, 1, "Snake IT!");
	mvwprintw(gWStats, 0, 1, "Stats");

	wrefresh(gWGame);
	wrefresh(gWStats);
	timeout(WAIT_TIME);
	
}

int main(void) {
	int key = -1;
	t_dir currDir = RIGHT;
	int continueGame = TRUE;
	initGame();
	t_pos start = {1, 1};
	listPtr_init ();
	
	while (continueGame) {
		key = getch();
		switch (key) {
			case 'q': continueGame = FALSE; break;
			case 'k': currDir = UP; break;
			case 'j': currDir = DOWN; break;
			case 'h': currDir = LEFT; break;
			case 'l': currDir = RIGHT; break;
			// case KEY_UP: currDir = UP; break;
			// case KEY_DOWN: currDir = DOWN; break;
			// case KEY_LEFT: currDir = LEFT; break;
			// case KEY_RIGHT: currDir = RIGHT; break;
			default: break;
		}
		switch (currDir) {
			case UP: start.line --; break;
			case DOWN: start.line ++; break;
			case RIGHT: start.col ++; break;
			case LEFT: start.col --; break;
		}
		if (start.line >= LINES - 3 - 1) start.line = 1;
		else if (start.line <= 0) start.line = LINES - 3 - 2;
		
		if (start.col >= COLS - 1) start.col = 1;
		else if (start.col <= 0) start.col = COLS - 2;
		mvwprintw(gWGame, start.line, start.col, "#");
		wrefresh(gWGame);
		mvwprintw(gWGame, start.line, start.col, " ");
	}
	getch();
	endwin();

	delwin(gWGame);
	delwin(gWStats);

	return 0;
}
