#include "config_bar.h"

#include "ncurses_funcs.h"

#include <cstring>

#define CONFIG_WIN_HEIGHT	1
//Width is dynamically defined.
#define CONFIG_WIN_START_Y	(LINES - 1)
#define CONFIG_WIN_START_X	0

config_bar::config_bar(){
	sprintf(buf, "test | showing: tcp | group-by: none | sound: n/a");
	buflen = strlen(buf);
	win = ncurses_funcs::create_newwin(CONFIG_WIN_HEIGHT, buflen, CONFIG_WIN_START_Y, CONFIG_WIN_START_X);
}

config_bar::~config_bar(){
	delwin(win);
}

void config_bar::draw(){
	resize();
	wclear(win);

	mvwprintw(win, 0, 0, buf);

	wrefresh(win);
}

void config_bar::resize(){
	mvwin(win, CONFIG_WIN_START_Y, CONFIG_WIN_START_X);
	wresize(win, CONFIG_WIN_HEIGHT, buflen);
}
