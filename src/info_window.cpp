#include "info_window.h"

#include "ncurses_funcs.h"

info_window::info_window(){
	win = ncurses_funcs::create_newwin(LINES/3, COLS, 0, 0);
}

info_window::~info_window(){
	delwin(win);
}
