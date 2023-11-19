#ifndef CONNECTION_ENTRY_H
#define CONNECTION_ENTRY_H

typedef struct connection_entry{
	const char* netid;
	const char* state;
	const char* local_addr;
	const char* rem_addr;
	const char* local_port;
	const char* rem_port;
} conn_entry;

#endif
