//Thanks to beej's guide to networking for providing a quality introduction to sockets.
//(And also example code.)

#include "networking_agent.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <arpa/inet.h>
#include <stdlib.h>

#include "debug.h"

networking_agent::networking_agent(){}

#define DEFAULT_PORT "40344"
#define BACKLOG 10
#define BUFLEN 1024

int networking_agent::init_socket(){
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	//Use the loopback address because node is NULL and AF_PASSIVE isn't specified.
	//Cf. man page of getaddrinfo().
	//hints.ai_flags = AF_PASSIVE;
	
	const char* port = DEFAULT_PORT;
	
	struct addrinfo* servinfo;
	struct addrinfo* p;
	int r;
	if((r = getaddrinfo(NULL, port, &hints, &servinfo)) != 0){
		dbgprint("getaddrinfo: %s\n", gai_strerror(r));
		return -1;
	}

	//Loop through all the results and bind to the first we can.
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((server_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			continue;
		}

		//Lose the pesky "address already in use" error message.
		int yes = 1;
		setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		//Set to non-blocking.
		if(fcntl(server_sock, F_SETFL, O_NONBLOCK) == -1){
			dbgprint("network_agent couldn't set socket to non-blocking for some reason.\n");
			return -1;
		}

		if(bind(server_sock, p->ai_addr, p->ai_addrlen) == -1){
			close(server_sock);
			continue;
		}

		break;
	}

	if(p == NULL){
		dbgprint("network_agent failed to bind a socket.\n");
		return -1;
	}

	dbgprint("Selected ai_family: %d\nSelected ai_socktype: %d\nSelected ai_protocol: %d\n", p->ai_family, p->ai_socktype, p->ai_protocol);

	freeaddrinfo(servinfo);

	//Now we listen().
	if(listen(server_sock, BACKLOG) == -1){
		dbgprint("network_agent failed to listen on its socket: %s.\n", strerror(errno));
		return -1;
	}

	return 0;
}

int networking_agent::init(){
	int r = init_socket();
	if(r == -1){
		return -1;
	}

	return 0;
}

networking_agent::~networking_agent(){
	for(int sock : clients){
		close(sock);
	}

	close(server_sock);
}

//Get sockaddr, IPv6 or IPv4:
void* get_in_addr(struct sockaddr* sa){
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int networking_agent::add_client(int client_socket){
	dbgprint("[NET_AGENT] Adding client connection.\n");

	//Set to non-blocking.
	if(fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1){
		dbgprint("network_agent couldn't set connected socket to non-blocking for some reason.\n");
		return -1;
	}

	clients.push_back(client_socket);

	return 0;
}

void networking_agent::rem_client(int idx){
	close(clients[idx]);
	clients.erase(clients.begin() + idx);
}

int networking_agent::check_for_incoming_connections(){
	int connected_socket;
	struct sockaddr_storage rem_addr;
	socklen_t sin_size = sizeof(rem_addr);

	connected_socket = accept(server_sock, (struct sockaddr*) &rem_addr, &sin_size);
	if(connected_socket == -1){
		if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR){
			//Non-blocking and didn't find connection this time.
			//We'll look for more connections to accept() next round.
			return 1;
		}
		else{
			dbgprint("Something went wrong with accepting a connection!\n");
			return -1;
		}
	}
	
	//Received a connection!
	char addr_str[INET6_ADDRSTRLEN];
	void* addr_bits = get_in_addr((struct sockaddr*) &rem_addr);
	dbgprint("Received a connection from %s.\n", inet_ntop(rem_addr.ss_family, addr_bits, addr_str, sizeof(addr_str)));

	int r = add_client(connected_socket);
	if(r == -1){
		return -1;
	}

	return 0;
}

#include "net_common.h"

int networking_agent::check_for_messages(message* message_array, unsigned int array_len){
	int num_bytes;
	char buf[BUFLEN];

	unsigned int midx = 0;

	unsigned int outstanding_infos = 0;
	unsigned int outstanding_warns = 0;
	unsigned int outstanding_alerts = 0;

	if(array_len == 0){
		return -2;
	}

	for(unsigned int i = 0; i < clients.size(); i++){
		int client_socket = clients[i];
		if((num_bytes = recv(client_socket, buf, BUFLEN, 0)) == -1){
			//Clients are non-blocking -- continue to the next client.
			if(errno == EAGAIN || errno == EWOULDBLOCK){
				continue;
			}

			//If for any other reason the client is erroring out,
			//just close the connection.
			rem_client(i);
			i--;
			continue;
		}

		//Connection was closed.
		if(num_bytes == 0){
			rem_client(i);
			i--;
			continue;
		}

		dbgprint("[NET_AGENT] Received some info! String is: %s\n", buf);

		if(strncmp("INFO", buf, 4) == 0){
			buf[BUFLEN - 1] = '\0';
			char* p = strstr(buf, "END");
			//No END in sight! Abandon this message; it is ill-formed.
			if(p == NULL){
				continue;
			}

			message_array[midx].type = MSG_TYPE_INFO;

			//							v	includes the '\0'
			char* new_text = (char*) calloc(p - buf, sizeof(char));
			strncpy(new_text, buf + 4, p - buf - 4);

			if(midx >= array_len){
				//Trying to assign to a full array!
				return -1;
			}

			if(p == buf + 4){
				//There is no text between INFO and END.
				message_array[midx].text = NULL;
			}
			else{
				message_array[midx].text = new_text;
			}
			midx++;

			dbgprint("[NET_AGENT] Received an info signal!\n");
			outstanding_infos++;

			if(midx >= array_len){
				//Reached limit; return early and leave potential messages in recv buffer.
				break;
			}
		}

		if(strncmp("WARN", buf, 4) == 0){
			dbgprint("[NET_AGENT] Classed a WARN signal; checking...\n");
			buf[BUFLEN - 1] = '\0';
			char* p = strstr(buf, "END");
			//No END in sight! Abandon this message; it is ill-formed.
			if(p == NULL){
				dbgprint("[NET_AGENT] Signal did not have an END! Discarding...\n");
				continue;
			}

			message_array[midx].type = MSG_TYPE_WARN;

			//							v	includes the '\0'
			char* new_text = (char*) calloc(p - buf, sizeof(char));
			strncpy(new_text, buf + 4, p - buf - 4);

			if(midx >= array_len){
				//Trying to assign to a full array!
				return -1;
			}

			if(p == buf + 4){
				//There is no text between WARN and END.
				message_array[midx].text = NULL;
			}
			else{
				message_array[midx].text = new_text;
			}
			midx++;

			dbgprint("[NET_AGENT] Received a warn signal!\n");
			outstanding_warns++;

			if(midx >= array_len){
				//Reached limit; return early and leave potential messages in recv buffer.
				break;
			}
		}

		if(strncmp("ALERT", buf, 5) == 0){
			char* p = strstr(buf, "END");
			//No END in sight! Abandon this message; it is ill-formed.
			if(p == NULL){
				continue;
			}

			message_array[midx].type = MSG_TYPE_ALERT;

			//							v	includes the '\0'
			char* new_text = (char*) calloc(p - buf, sizeof(char));
			strncpy(new_text, buf + 5, p - buf - 5);
dbgprint("[NET_AGENT] copied buf to new_text: %s\n", new_text);

			if(midx >= array_len){
				//Trying to assign to a full array!
				return -1;
			}

			if(p == buf + 5){
				//There is no text between ALERT and END.
				message_array[midx].text = NULL;
			}
			else{
				message_array[midx].text = new_text;
			}
			midx++;

			dbgprint("[NET_AGENT] Received an alert signal!\n");
			outstanding_alerts++;

			if(midx >= array_len){
				//Reached limit; return early and leave potential messages in recv buffer.
				break;
			}
		}
	}

	return outstanding_infos + outstanding_warns + outstanding_alerts;
}
