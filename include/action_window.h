#ifndef ACTION_WINDOW_H
#define ACTION_WINDOW_H

#include <ncurses.h>

#include "abstract_window.h"

class action_window : public abstract_window{
	public:
		action_window();
		~action_window();

		void show_actions(long unsigned int num_actions);
	private:
		WINDOW* win;
};

#endif
