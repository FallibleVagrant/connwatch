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
	private:
		WINDOW* win;
};

#endif
