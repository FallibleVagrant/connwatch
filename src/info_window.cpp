#include "info_window.h"

#include "ncurses_funcs.h"

#include "common.h"
#include "debug.h"

info_window::info_window(){
	win = ncurses_funcs::create_newwin(LINES/3, COLS, 0, 0);
}

info_window::~info_window(){
	delwin(win);
}

void info_window::draw(){
	wclear(win);
	box(win, 0, 0);
	mvwprintw(win, 1, 1, "Info window!");

	const char* ellipsis;
	switch(ticker){
		case 0:
			ellipsis = ".";
			break;
		case 1:
			ellipsis = "..";
			break;
		case 2:
			ellipsis = "...";
			break;
		default:
			dbgprint("Impossible situation reached! Expect a crash!\n");
			break;
	}
	mvwprintw(win, 2, 1, ellipsis);

	wrefresh(win);
}

void info_window::resize(){
	mvwin(win, 0, 0);
	wresize(win, LINES/3, COLS);
}
