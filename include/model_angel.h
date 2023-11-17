#ifndef MODEL_ANGEL_H
#define MODEL_ANGEL_H

//#include "window_demon.h"

struct slabstat {
	int socks;
	int tcp_ports;
	int tcp_tws;
	int tcp_syns;
	int skbs;
};

class model_angel{
	public:
		model_angel();
		~model_angel();

		int update();
	private:
		struct slabstat slabstat;
		//window_demon demon;

		int get_good_buffer(char** buf, int* bufsize);
		int fetch_tcp_data();
		//int fetch_udp_data();
		//int fetch_raw_data();
		int fetch_connections();
};

#endif
