#include "connection_entry.h"

#include <stdlib.h>

void free_conn_entry(conn_entry* c){
	free(c->read_queue);
	free(c->write_queue);

	free(c->local_hostname);
	free(c->local_addr);
	free(c->local_service);
	free(c->local_port);

	free(c->rem_hostname);
	free(c->rem_addr);
	free(c->rem_service);
	free(c->rem_port);
	
	free(c);
}
