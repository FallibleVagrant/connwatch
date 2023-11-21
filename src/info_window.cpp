#include "info_window.h"

#include "ncurses_funcs.h"

#include "common.h"
#include "debug.h"

#define INFO_WIN_HEIGHT		(LINES / 3)
#define INFO_WIN_WIDTH		COLS
#define INFO_WIN_START_Y	0
#define INFO_WIN_START_X	0

info_window::info_window(){
	win = ncurses_funcs::create_newwin(INFO_WIN_HEIGHT, INFO_WIN_WIDTH, INFO_WIN_START_Y, INFO_WIN_START_X);
	message = NULL;
}

info_window::~info_window(){
	delwin(win);
}

void info_window::draw(){
	resize();
	wclear(win);
	box(win, 0, 0);
	if(message != NULL){
		mvwprintw(win, 1, 1, message);
	}
	else{
		mvwprintw(win, 1, 1, "Info window!");
	}

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
	mvwin(win, INFO_WIN_START_Y, INFO_WIN_START_X);
	wresize(win, INFO_WIN_HEIGHT, INFO_WIN_WIDTH);
}

void info_window::print(const char* message){
	this->message = message;
}
