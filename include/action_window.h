#ifndef ACTION_WINDOW_H
#define ACTION_WINDOW_H

#include <ncurses.h>

#include "abstract_window.h"

class action_window : public abstract_window{
	public:
		action_window();
		~action_window();

		void toggle_visibility();
		bool is_visible;
		void draw();
		void select_down();
		void select_up();
		void resize();
	private:
		WINDOW* win;
};

#endif
