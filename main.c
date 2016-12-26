#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include "list_ptr.h"
// penser au COLOR_PAIR

/**************************/
/* Parameters of the Game */
/**************************/

#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a > b) ? b : a)
#define abs(a) ((a > 0) ? a : -a)
#define randab(a, b) ( (rand() % abs(b - a)) + min(a, b))

#define WAIT_TIME 50       // the time to wait for a user input
#define GROWTH 5           // length's gain when one food is eat
#define MAX_FOOD 10        // Maximum food quantity at the same time
#define RANDOMIZE FALSE

// they both take a value in initGame()
int CAN_CROWL_ON_HIM;      // obvious
int CAN_GO_THROUGH_BORDER; // "

WINDOW *gWGame, *gWStats;
typedef enum {UP, DOWN, LEFT, RIGHT} t_dir;
int justForKonami = -1;

// initialize the ncurses library, the random, and print the screen
void initGame () {
	timeout(WAIT_TIME); // if no input then dont block
	//cbreak();
	start_color();
	init_pair(1 , 28,   COLOR_BLACK);
	keypad(stdscr, TRUE); // use of the arrow keys
	noecho(); // no echo (obvious :p)
	curs_set(0); // no cursor
	
	gWGame= subwin(stdscr, LINES - 3, COLS, 0, 0);
	gWStats= subwin(stdscr, 3, COLS, LINES - 3, 0);
	
	box(gWGame, ACS_VLINE, ACS_HLINE);
	box(gWStats, ACS_VLINE, ACS_HLINE);

	mvwprintw(gWGame, 0, 1, " Snake IT! --- <> with ♥ by T2 --- ");
	mvwprintw(gWStats, 0, 1, " Stats ");

	wrefresh(gWGame);
	wrefresh(gWStats);
	
	CAN_CROWL_ON_HIM      = (rand()%2 == 0) ? TRUE : FALSE;
	CAN_GO_THROUGH_BORDER = (rand()%2 == 0) ? TRUE : FALSE;
}

// display the snake
void displaySnake (int isAlive, int currDir) {
	t_pos pos;
	
	listPtr_move2head();
	listPtr_next ();
	while (!listPtr_isOut ()) {
		listPtr_readData (&pos);
		switch (pos.body) {
			case HORIZONTAL:   mvwprintw(gWGame, pos.line, pos.col, "═"); break;
			case VERTICAL:     mvwprintw(gWGame, pos.line, pos.col, "║"); break;
			case TOP_LEFT:     mvwprintw(gWGame, pos.line, pos.col, "╔"); break;
			case TOP_RIGHT:    mvwprintw(gWGame, pos.line, pos.col, "╗"); break;
			case BOTTOM_LEFT:  mvwprintw(gWGame, pos.line, pos.col, "╚"); break;
			case BOTTOM_RIGHT: mvwprintw(gWGame, pos.line, pos.col, "╝"); break;
		}
		listPtr_next();
	}
	
	listPtr_move2end();
	listPtr_readData(&pos);
	
	mvwprintw(gWGame, pos.line, pos.col, "*");
	
	listPtr_move2head();
	listPtr_readData (&pos);
	if (isAlive) {
		switch (currDir) {
			case UP:    mvwprintw(gWGame, pos.line, pos.col, "^"); break;
			case DOWN:  mvwprintw(gWGame, pos.line, pos.col, "v"); break;
			case RIGHT: mvwprintw(gWGame, pos.line, pos.col, ">"); break;
			case LEFT:  mvwprintw(gWGame, pos.line, pos.col, "<"); break;
		}
	}
	else mvwprintw(gWGame, pos.line, pos.col, "!");
}

// display some information about the game
void displayStats(int foodEat, int length, int foodQtt) {
	mvwprintw(gWStats, 1, 1, "foodEat: %d, length: %d, foodQtt: %d, cannibalism(c): %3s, ghost(g): %3s | 'q' to quit | 'r' to replay", foodEat, length, foodQtt, CAN_CROWL_ON_HIM ? "YES" : "NO", CAN_GO_THROUGH_BORDER ? "YES" : "NO");
	wrefresh(gWStats);
}

// display all the food
void displayFood(t_pos * foods, int foodQtt) {
	int i;
	for (i = 0; i < foodQtt; i++) {
		wattron(gWGame, COLOR_PAIR(1));
		mvwprintw(gWGame, foods[i].line, foods[i].col, "♥");
		wattroff(gWGame, COLOR_PAIR(1));
	}
}

// add a random food at the i position
void randomFood(t_pos * foods, int i) {
	foods[i].line = rand()%(LINES - 3 - 2) + 1;
	foods[i].col  = rand()%(COLS - 2) + 1;
}

// return TRUE if the snake eat and update some stuff if he did
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

void randPosOnWall (t_pos * pos, t_dir * currDir) {
	int wall = rand()%4;
	int bounce = rand()%2;
	if (bounce == 0) {
		switch (wall) {
			case 0:
				*currDir = UP;
				pos -> line = LINES - 5;
				pos -> col = randab(1, COLS - 2);
				break;

			case 1:
				*currDir = DOWN;
				pos -> line = 1;
				pos -> col = randab(1, COLS - 2);
				break;

			case 2:
				*currDir = RIGHT;
				pos -> line = randab(1, LINES - 5);
				pos -> col = 1;
				break;

			case 3:
				*currDir = LEFT;
				pos -> line = randab(1, LINES - 5);
				pos -> col = COLS - 2;
				break;
		}
	}
	else {
		// *currDir = (*currDir == UP) ? DOWN : (*currDir == DOWN) ? UP : (*currDir == RIGHT) ? LEFT : RIGHT; // not eazy to understand..
		switch (*currDir) {
			case UP:
				*currDir = DOWN;
				(pos -> line)++;
				break;

			case DOWN:
				*currDir = UP;
				(pos -> line)--;
				break;

			case RIGHT:
				*currDir = LEFT;
				(pos -> col)--;
				break;

			case LEFT:
				*currDir = RIGHT;
				(pos -> col)++;
				break;
		}
	}
}

void endGame() {
	int i, j;
	for (i = 0; i < LINES; i++)
		for (j = 0; j < COLS; j++)
			mvprintw (i, j, " ");

	delwin(gWGame);
	delwin(gWStats);
}

int snake(void) {
	/**********************************/
	/* Declaration and Initialisation */
	/**********************************/
	int key = -1,
		foodEat = 0,
		length = 1,
		growth = 0,
		foodQtt = 1,
		continueGame = TRUE;

	t_dir currDir = RIGHT,
		  prevDir;

	t_pos head = {1, 1, HORIZONTAL},
		  prev,
		  end,
		  foods[MAX_FOOD];
	
	initGame();
	randomFood(foods, 0);
	
	listPtr_init ();
	listPtr_appendHead (head);
	
	/********************/
	/* Loop of the Game */
	/********************/
	
	while (continueGame) {
		// listen for the next player's action
		key = getch();
		prevDir = currDir;
		switch (key) {
			case 'r':
			case 'q': // quit
				continueGame = FALSE;
				break;
			
			case ' ':
				growth += 1;
				break;
			
			case 'g':
				CAN_GO_THROUGH_BORDER = !CAN_GO_THROUGH_BORDER;
				break;

			case 'c':
				CAN_CROWL_ON_HIM = !CAN_CROWL_ON_HIM;
				break;
			
			// the player can't go to the opposite direction
			case KEY_UP: // go up
				if (currDir != DOWN) currDir = UP;
				break;
				
			case KEY_DOWN: // go down
				if (currDir != UP) currDir = DOWN;
				break;
				
			case KEY_LEFT: // go left
				if (currDir != RIGHT) currDir = LEFT;
				break;
				
			case KEY_RIGHT: // go right
				if (currDir != LEFT) currDir = RIGHT;
				break;
				
			default: // anything else ? just continue
				break;
		}
		
		listPtr_move2head();
		listPtr_readData (&head);
		prev = head;
		
		// updating the position of the head
		switch (currDir) {
			case UP: head.line --; break;
			case DOWN: head.line ++; break;
			case RIGHT: head.col ++; break;
			case LEFT: head.col --; break;
		}
		
		// find the symbol to represent the part nex to the head
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
		// update the part next to the head
		listPtr_move2head();
		listPtr_removeElt (); // an update element primitive should be nice
		listPtr_appendHead (prev);
		
		// is it a legal movment ?
		if (
			(
				head.line >= LINES - 3 - 1 ||
				head.line <= 0 ||
				head.col >= COLS - 1 ||
				head.col <= 0
			) && CAN_GO_THROUGH_BORDER == FALSE
		) continueGame = FALSE;
		else {
			if (head.line >= LINES - 3 - 1 || head.line <= 0 || head.col >= COLS - 1 || head.col <= 0) {

				if (
					CAN_GO_THROUGH_BORDER &&
					CAN_CROWL_ON_HIM &&
					RANDOMIZE
				) randPosOnWall(&head, &currDir);
				else {
					if (head.line >= LINES - 3 - 1) head.line = 1;
					else if (head.line <= 0) head.line = LINES - 3 - 2;

					if (head.col >= COLS - 1) head.col = 1;
					else if (head.col <= 0) head.col = COLS - 2;
				}
			}



			if (eat(head, foods, foodQtt, &foodEat, &growth) && foodQtt < MAX_FOOD) {
				randomFood(foods, foodQtt);
				foodQtt++;
			}

			if (growth == 0) {
				listPtr_move2end ();
				listPtr_readData(&end);
				mvwprintw(gWGame, end.line, end.col, " ");
				listPtr_removeElt ();
			} else {
				growth--;
				length++;
			}


			if (listPtr_isInList(head) && CAN_CROWL_ON_HIM == FALSE) {
				continueGame = FALSE;
			} else {
				listPtr_appendHead (head);

				displaySnake(continueGame, currDir);
				displayFood(foods, foodQtt);
				wrefresh(gWGame);

				displayStats(foodEat, length, foodQtt);
			}
		}
	}
	
	/*******************/
	/* End of the Game */
	/*******************/
	
	listPtr_move2end ();
	listPtr_readData(&end);
	mvwprintw(gWGame, end.line, end.col, " ");
	listPtr_removeElt ();
	
	listPtr_appendHead (head);
	
	displaySnake(continueGame, currDir);
	wrefresh(gWGame);
	
	listPtr_removeList ();
	
	displayStats(foodEat, length, foodQtt);
	
	timeout(-1);
	while (key != 'q') {
		if (key == 'r') {
			endGame();
			return 1;
		}
		key = getch();
	}
	
	endGame();
	return 0;
}

int konami (int key) {
	int code[10] = {KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT, 'b', 'a'};
	justForKonami++;
	if (key == code[justForKonami]) {
		if (key == 'a') {
			snake();
			refresh();
			return TRUE;
		}
	}
	else {
		if (key == KEY_UP)justForKonami = 0;
		else justForKonami = -1;
	}
	return FALSE;
}

int main () {
	srand(time(NULL));
	setlocale(LC_ALL, "");
	initscr();
	while(snake());
	endwin();
	return 0;
}
