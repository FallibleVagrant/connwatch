#ifndef CONNECTIONS_WINDOW_H
#define CONNECTIONS_WINDOW_H

#include <ncurses.h>
#include <vector>

#include "connection_entry.h"
#include "abstract_window.h"

class connections_window : public abstract_window{
	public:
		connections_window();
		~connections_window();

		void draw();
		void update_connections(const std::vector<conn_entry*>& connections);
		void select_down();
		void select_up();
		void resize();
	private:
		WINDOW* win;
		std::vector<conn_entry*> connections;
		int choice;
};

#endif
