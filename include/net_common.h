#ifndef NET_COMMON_H
#define NET_COMMON_H

enum{
	INFO,
	WARN,
	ALERT,
};

extern const char* MSG_TYPES[];

//For communicating between networking agent and angel.
typedef struct message{
	int type;
	char* text;
} message;

#endif
