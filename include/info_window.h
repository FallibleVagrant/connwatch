#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include <ncurses.h>

class info_window{
	public:
		info_window();
		~info_window();
	private:
		WINDOW* win;
};

#endif
