#ifndef CONNECTIONS_WINDOW_H
#define CONNECTIONS_WINDOW_H

#include <ncurses.h>
#include <vector>

#include "connection_entry.h"

class connections_window{
	public:
		connections_window();
		~connections_window();

		void show_connections(const std::vector<connection_entry*> connections);
	private:
		WINDOW* win;
};

#endif
