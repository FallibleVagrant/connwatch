#ifndef NETWORKING_AGENT_H
#define NETWORKING_AGENT_H

#define MAX_CLIENTS 1024

#include <vector>
#include "net_common.h"

class networking_agent{
	public:
		networking_agent();
		~networking_agent();

		int init();
		int check_for_incoming_connections();
		int check_for_messages(message* message_array, unsigned int array_len);
		void reset_outstanding_warnings();
	private:
		int server_sock;
		std::vector<int> clients;

		int init_socket();
		int add_client(int fd);
		void rem_client(int idx);
};

#endif
