#ifndef CONFIG_BAR_H
#define CONFIG_BAR_H

#include <ncurses.h>

#include "abstract_window.h"

class config_bar : abstract_window{
	public:
		config_bar();
		~config_bar();

		void draw();
		void resize();
		void toggle_hostnames();
		void toggle_services();
		void toggle_sound();
		void cycle_sort_method();
	private:
		WINDOW* win;
};

#endif
