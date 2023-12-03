#ifndef PROC_READER_H
#define PROC_READER_H

#include <vector>
#include <stdio.h>

#include "connection_entry.h"
#include "dns_cache.h"

struct slabstat {
	int socks;
	int tcp_ports;
	int tcp_tws;
	int tcp_syns;
	int skbs;
};

struct __inet_prefix;
typedef __inet_prefix inet_prefix;

//For reading /proc/net/whatever and also handling DNS requests.
class proc_reader{
	public:
		proc_reader();
		~proc_reader();

		int init();
		int fetch_connections(std::vector<conn_entry*>& connections);
	private:
		struct slabstat slabstat;
		std::vector<conn_entry*>* connections_pointer;
		dns_cache cache;

		int get_good_buffer(char** buf, int* bufsize);
		int fetch_tcp_data();
		int fetch_udp_data();
		//int fetch_raw_data();

		int open_generic_proc(FILE* fp, int AF, int netid);
		int tcp_parse_proc_line(char* line, int AF);
		int open_proc_tcp(FILE* fp, int AF);
		int udp_parse_proc_line(char* line, int AF);
		int open_proc_udp(FILE* fp, int AF);

		char* resolve_hostname(const inet_prefix* addr, int port);
};

#endif
