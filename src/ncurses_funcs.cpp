#include "ncurses_funcs.h"

WINDOW* ncurses_funcs::create_newwin(int height, int width, int starty, int startx){
	WINDOW* local_win;

	local_win = newwin(height, width, starty, startx);
	//box(local_win, 0, 0);
	
	//wrefresh(local_win);

	return local_win;
}
