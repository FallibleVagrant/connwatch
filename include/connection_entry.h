#ifndef CONNECTION_ENTRY_H
#define CONNECTION_ENTRY_H

typedef struct connection_entry{
	const char* netid;
	const char* state;
	char* local_addr;
	char* rem_addr;
} conn_entry;

#endif
