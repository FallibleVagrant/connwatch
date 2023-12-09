#ifndef NETWORKING_AGENT_H
#define NETWORKING_AGENT_H

#define MAX_CLIENTS 1024

#include <vector>
#include "net_common.h"
#include "connection_entry.h"

struct client{
	unsigned long long client_id;
	int client_socket;
	bool has_requested_ips;
};

class networking_agent{
	public:
		networking_agent();
		~networking_agent();

		int init();
		int check_for_incoming_connections(std::vector<char*>& added_connection_msgs);
		int check_for_messages(std::vector<message>& messages);
		void reset_outstanding_warnings();
		int send_requested_ips(std::vector<conn_entry*>& connections);
	private:
		int server_sock;
		unsigned long long seq_client_id;
		std::vector<struct client> clients;

		int init_socket();
		int add_client(int fd);
		void rem_client_by_index(int idx);
};

#endif
