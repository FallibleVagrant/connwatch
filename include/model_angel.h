#ifndef MODEL_ANGEL_H
#define MODEL_ANGEL_H

#include "connection_entry.h"

#include <vector>

struct slabstat {
	int socks;
	int tcp_ports;
	int tcp_tws;
	int tcp_syns;
	int skbs;
};

class window_demon;

class model_angel{
	public:
		model_angel();
		void start(window_demon* pointer_to_demon);
		~model_angel();

		int update();
		std::vector<conn_entry*> get_connections();
		int get_num_warnings();
		int get_num_alerts();
		unsigned int get_num_connections();
		void warn();
		void alert();
	private:
		struct slabstat slabstat;
		window_demon* demon_pointer;
		std::vector<conn_entry*> connections;
		int num_warnings;
		int num_alerts;

		int get_good_buffer(char** buf, int* bufsize);
		int fetch_tcp_data();
		//int fetch_udp_data();
		//int fetch_raw_data();
		int fetch_connections();
};

#endif
