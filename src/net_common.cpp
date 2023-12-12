#include "net_common.h"

const char* MSG_TYPES[] = {
	[INFO] = "INFO",
	[WARN] = "WARN",
	[ALERT] = "ALERT",
	[REQ_IP] = "REQ_IP",
};

time_t time_since_last_connection_update = 0;
//3 secs b/t updates.
unsigned int time_between_connection_updates = 3;

time_t time_since_last_ip_reqs_update = time(NULL);
//10 secs b/t updates.
unsigned int time_between_ip_reqs_updates = 10;
