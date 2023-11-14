#ifndef WINDOW_DEMON_H
#define WINDOW_DEMON_H

#include <string>
#include <vector>

#include "connection_entry.h"

#include "info_window.h"
#include "connections_window.h"
#include "action_window.h"

class window_demon{
	public:
		window_demon();
		~window_demon();

		void update_connections(const std::vector<std::string*>& new_connections);
	private:
		info_window info_win;
		connections_window connect_win;
		action_window action_win;

		std::vector<connection_entry*> connections;
};

#endif
