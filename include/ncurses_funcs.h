#ifndef NCURSES_FUNCS_H
#define NCURSES_FUNCS_H

#include <ncurses.h>

class ncurses_funcs{
	public:
		static WINDOW* create_newwin(int height, int width, int starty, int startx);
};

#endif
