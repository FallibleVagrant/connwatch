#include "config_bar.h"

#include "ncurses_funcs.h"

config_bar::config_bar(){
	win = ncurses_funcs::create_newwin(1, 50, LINES-1, 0);
}

config_bar::~config_bar(){
	delwin(win);
}

void config_bar::draw(){
	resize();
	wclear(win);

	mvwprintw(win, 0, 0, "test | showing: tcp | group-by: none | sound: n/a");

	wrefresh(win);
}

void config_bar::resize(){
	mvwin(win, LINES-1, 0);
	wresize(win, 1, 50);
}
