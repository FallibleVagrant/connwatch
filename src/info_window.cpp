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
	has_custom_message = false;
}

info_window::~info_window(){
	delwin(win);
}

void info_window::draw(){
	resize();
	//wclear(win);
	//Jankily clear the screen.
	for(int i = 2; i < INFO_WIN_HEIGHT; i++){
		mvwprintw(win, i, 0, "                                                                                                           ");
	}
	box(win, 0, 0);
	if(!has_custom_message){
		mvwprintw(win, 1, 1, "Info window!");
	}

	const char* ellipsis;
	switch(ticker % 3){
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

void info_window::print(const char* fmt, ...){
	va_list args;
	va_start(args, fmt);
	mvwprintw(win, 1, 1, fmt, args);
	va_end(args);
	//Clear the space afterwards.
	wprintw(win, "          ");
	has_custom_message = true;
}
