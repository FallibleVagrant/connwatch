#ifndef WARN_WINDOW_H
#define WARN_WINDOW_H

#include <ncurses.h>

#include "abstract_window.h"

class model_angel;

class warn_window : public abstract_window{
	public:
		warn_window();
		void start(model_angel* pointer_to_angel);
		~warn_window();

		void draw();
		void resize();
	private:
		WINDOW* win;
		model_angel* angel_pointer;
};

#endif
