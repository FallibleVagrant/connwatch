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
		void select_right();
		void select_left();
		void select_down();
		void select_up();
		void trigger_resize();
		void enter_bogus_mode();
		void add_bogus_entry();
		void rem_bogus_entry();
	private:
		//model_angel& angel;

		info_window info_win;
		connections_window connect_win;
		action_window action_win;
		config_bar config_win;

		bool bogus_mode;
		int bogus_entries = 0;
		std::vector<conn_entry*> bogus_connections;
};

#endif
