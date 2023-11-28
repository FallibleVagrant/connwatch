#ifndef NET_COMMON_H
#define NET_COMMON_H

//For communicating between networking agent and angel.
typedef struct message{
	int type;
	char* text;
} message;

#define MSG_TYPE_INFO	0
#define MSG_TYPE_WARN	1
#define MSG_TYPE_ALERT	2

#endif
