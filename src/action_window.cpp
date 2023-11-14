#include "action_window.h"

#include "ncurses_funcs.h"

action_window::action_window(){
	win = ncurses_funcs::create_newwin((2*LINES/3)+2, (COLS/3)+1, (LINES/3)-1, 2*COLS/3);
}

action_window::~action_window(){
	delwin(win);
}

void action_window::show_actions(long unsigned int num_actions){
	int y;
	int x;
	getmaxyx(win, y, x);

	for(long unsigned int i = 0; i < num_actions; i++){
		if(i > (long unsigned int) y - 1){
			break;
		}
		mvwprintw(win, (int) i + 1, 1, "Action!");
	}

	wrefresh(win);
}
