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

		void draw(const std::vector<conn_entry*>& connections);
	private:
		WINDOW* win;
};

#endif
