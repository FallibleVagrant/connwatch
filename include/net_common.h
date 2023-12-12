#ifndef NET_COMMON_H
#define NET_COMMON_H

enum{
	INFO,
	WARN,
	ALERT,
	REQ_IP,
};

extern const char* MSG_TYPES[];

//For communicating between networking agent and angel.
typedef struct message{
	int type;
	char* text;
} message;

#include <time.h>

extern time_t time_since_last_connection_update;
extern unsigned int time_between_connection_updates;

extern time_t time_since_last_ip_reqs_update;
extern unsigned int time_between_ip_reqs_updates;

#endif
