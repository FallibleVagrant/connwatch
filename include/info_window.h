#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include <ncurses.h>

#include "abstract_window.h"

class info_window : public abstract_window{
	public:
		info_window();
		~info_window();

		void draw();
		void resize();
		void print(const char* message);
	private:
		WINDOW* win;
		const char* message;
};

#endif
