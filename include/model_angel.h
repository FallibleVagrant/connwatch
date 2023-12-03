#ifndef MODEL_ANGEL_H
#define MODEL_ANGEL_H

#include "connection_entry.h"

#include <vector>
#include <stdio.h>

class window_demon;

#include "networking_agent.h"
#include "proc_reader.h"

class model_angel{
	public:
		model_angel();
		int start(window_demon* pointer_to_demon);
		~model_angel();

		int update();
		std::vector<conn_entry*> get_connections();
		std::vector<message*> get_log();
		int get_num_warnings();
		int get_num_alerts();
		unsigned int get_num_connections();
		void warn();
		void alert(const char* message);
		void reset_alerts_and_warns();
		void add_message_to_log(int type, char* text);
	private:
		window_demon* demon_pointer;
		std::vector<conn_entry*> connections;
		std::vector<message*> message_log;

		int num_warnings;
		int num_alerts;

		networking_agent net_agent;
		proc_reader proc_read;

		int check_networking_agent();
		void sort_connections();
};

#endif
