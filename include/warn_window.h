#ifndef WARN_WINDOW_H
#define WARN_WINDOW_H

#include <ncurses.h>

#include "abstract_window.h"

class warn_window : public abstract_window{
	public:
		warn_window();
		~warn_window();

		void draw();
		void resize();
	private:
		WINDOW* win;
		int warning_level;
};

#endif
