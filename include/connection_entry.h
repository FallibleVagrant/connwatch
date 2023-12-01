#ifndef CONNECTION_ENTRY_H
#define CONNECTION_ENTRY_H

typedef struct connection_entry{
	const char* netid;
	const char* state;
	int ip_ver;

	char* local_hostname;
	char* local_addr;
	char* local_service;
	char* local_port;

	char* rem_hostname;
	char* rem_addr;
	char* rem_service;
	char* rem_port;
} conn_entry;

void free_conn_entry(conn_entry* entry);

#endif
