#ifndef WINDOW_DEMON_H
#define WINDOW_DEMON_H

#include <string>
#include <vector>

#include "connection_entry.h"

#include "info_window.h"
#include "connections_window.h"
#include "action_window.h"
#include "config_bar.h"

class window_demon{
	public:
		window_demon();
		~window_demon();

		int update();
		void update_connections(const std::vector<conn_entry*>& connections);
		void show_actions();
		void hide_actions();
		void select_down();
		void select_up();
		void trigger_resize();
	private:
		//model_angel& angel;

		info_window info_win;
		connections_window connect_win;
		action_window action_win;
		config_bar config_win;
};

#endif
