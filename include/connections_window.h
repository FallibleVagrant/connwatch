#ifndef CONNECTIONS_WINDOW_H
#define CONNECTIONS_WINDOW_H

#include <ncurses.h>

#include "connection_entry.h"
#include "abstract_window.h"

#include "model_angel.h"

class connections_window : public abstract_window{
	public:
		connections_window();
		void start(model_angel* pointer_to_angel);
		~connections_window();

		void draw();
		void select_down();
		void select_up();
		void resize();
	private:
		WINDOW* win;
		model_angel* angel_pointer;
		unsigned int choice;
};

#endif
