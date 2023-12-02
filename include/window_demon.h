#ifndef WINDOW_DEMON_H
#define WINDOW_DEMON_H

#include <string>
#include <vector>

#include "connection_entry.h"

#include "info_window.h"
#include "connections_window.h"
#include "action_window.h"
#include "config_bar.h"
#include "warn_window.h"
#include "alarum_window.h"
#include "log_window.h"

class model_angel;

class window_demon{
	public:
		window_demon();
		int start(model_angel* pointer_to_angel);
		~window_demon();

		int update();
		void resolve_action(const char* selection);

		void select_right();
		void select_left();
		void select_down();
		void select_up();
		void trigger_resize();

		void trigger_alarum_popup(const char* alarum_message);

		void show_connections();
		void show_log();

		void toggle_hostnames();
		void toggle_services();
		void toggle_sound();
	private:
		model_angel* angel_pointer;

		info_window info_win;
		connections_window connect_win;
		action_window action_win;
		config_bar config_win;
		warn_window warn_win;
		alarum_window alarum_win;
		log_window log_win;

		//void show_main_window(main_window_type);
};

#endif
