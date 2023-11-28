#ifndef LOG_WINDOW_H
#define LOG_WINDOW_H

#include <ncurses.h>

#include "abstract_window.h"

#include "model_angel.h"

class log_window : public abstract_window{
	public:
		log_window();
		void start(model_angel* pointer_to_angel);
		~log_window();

		void draw();
		void select_down();
		void select_up();
		void resize();
	private:
		WINDOW* win;
		model_angel* angel_pointer;
		int choice;
};

#endif
