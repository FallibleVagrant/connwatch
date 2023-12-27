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
#include "common.h"

networking_agent::networking_agent(){}

#define DEFAULT_PORT "40344"
#define BACKLOG 10
#define BUFLEN 1024

int networking_agent::init_socket(){
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	//Use the loopback address because node is NULL and AI_PASSIVE isn't specified.
	//Cf. man page of getaddrinfo().
	//hints.ai_flags = AI_PASSIVE;
	
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
			dbgprint("[NET_AGENT] Couldn't set socket to non-blocking for some reason.\n");
			return -1;
		}

		if(bind(server_sock, p->ai_addr, p->ai_addrlen) == -1){
			close(server_sock);
			continue;
		}

		break;
	}

	if(p == NULL){
		dbgprint("[NET_AGENT] Failed to bind a socket.\n");
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
	for(struct client c : clients){
		close(c.client_socket);
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

#include <limits.h>

int networking_agent::add_client(int client_socket){
	dbgprint("[NET_AGENT] Adding client connection.\n");

	//Set to non-blocking.
	if(fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1){
		dbgprint("[NET_AGENT] Couldn't set connecting socket to non-blocking for some reason.\n");
		return -1;
	}

	struct client c = {seq_client_id, client_socket, false};
	clients.push_back(c);

	seq_client_id++;
	if(seq_client_id >= ULLONG_MAX){
		dbgprint("[NET_AGENT] Somehow, more than 2^64 - 1 clients connected to this process in its lifetime. Exiting...\n");
		abort();
	}

	return 0;
}

void networking_agent::rem_client_by_index(int idx){
	struct client client = clients[idx];

	int client_socket = client.client_socket;
	close(client_socket);
	clients.erase(clients.begin() + idx);
}

int networking_agent::check_for_incoming_connections(std::vector<char*>& added_connection_msgs){
	int connected_socket;
	struct sockaddr_storage rem_addr;
	socklen_t sin_size = sizeof(rem_addr);

	connected_socket = accept(server_sock, (struct sockaddr*) &rem_addr, &sin_size);
	if(connected_socket == -1){
		if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR){
			//Non-blocking and didn't find connection this time.
			//We'll look for more connections to accept() next round.
			return 0;
		}
		else{
			dbgprint("[NET_AGENT] Something went wrong with accepting a connection!\n");
			return -1;
		}
	}

	//Received a connection!
	char addr_str[INET6_ADDRSTRLEN];
	void* addr_bits = get_in_addr((struct sockaddr*) &rem_addr);
	dbgprint("[NET_AGENT] Connection received from %s.\n", inet_ntop(rem_addr.ss_family, addr_bits, addr_str, sizeof(addr_str)));

	char* msg_str = (char*) kalloc(128, sizeof(char));
	strcpy(msg_str, "Connection received from ");
	strcat(msg_str, addr_str);
	added_connection_msgs.push_back(msg_str);

	int r = add_client(connected_socket);
	if(r == -1){
		return -1;
	}

	//Send IPs to subscribed processes, as there is a new subscribed process.
	//TODO: Make this individualized?
	time_since_last_ip_reqs_update = 0;

	return 1;
}

#include "net_common.h"

//Recv()'d messages should be of format:
//INFO[ Message ]END
//WARN[ Message ]END
//ALERT[ Message ]END
//REQ IP
//
//Where [ Message ] is a string of characters of any length, including zero.
//TYPE and END are case-sensitive.
//END must be included.
//Bytes should not exceed 256, though for now that is not checked.
//Subject to change to a more reasonable design, like having the length of the message in the header.
int networking_agent::check_for_messages(std::vector<message>& messages){
	int num_bytes;
	char buf[BUFLEN];

	unsigned int outstanding_infos = 0;
	unsigned int outstanding_warns = 0;
	unsigned int outstanding_alerts = 0;
	unsigned int outstanding_reqs = 0;

	for(unsigned int i = 0; i < clients.size(); i++){
		int client_socket = clients[i].client_socket;
		if((num_bytes = recv(client_socket, buf, BUFLEN, 0)) == -1){
			//Clients are non-blocking -- continue to the next client.
			if(errno == EAGAIN || errno == EWOULDBLOCK){
				continue;
			}

			//If for any other reason the client is erroring out,
			//just close the connection.
			rem_client_by_index(i);
			i--;
			continue;
		}

		//Connection was closed.
		if(num_bytes == 0){
			rem_client_by_index(i);
			i--;
			continue;
		}

		buf[BUFLEN - 1] = '\0';
		dbgprint("[NET_AGENT] Received some info! String is: %s\n", buf);

		if(strncmp("INFO", buf, 4) == 0){
			char* p = strstr(buf, "END");
			//No END in sight! Abandon this message; it is ill-formed.
			if(p == NULL){
				continue;
			}

			message msg;
			msg.type = INFO;

			//							v	includes the '\0'
			char* new_text = (char*) kalloc(p - buf, sizeof(char));
			strncpy(new_text, buf + 4, p - buf - 4);

			if(p == buf + 4){
				//There is no text between INFO and END.
				msg.text = NULL;
			}
			else{
				msg.text = new_text;
			}

			messages.push_back(msg);

			dbgprint("[NET_AGENT] Received an info signal!\n");
			outstanding_infos++;
		}

		if(strncmp("WARN", buf, 4) == 0){
			dbgprint("[NET_AGENT] Classed a WARN signal; checking...\n");
			char* p = strstr(buf, "END");
			//No END in sight! Abandon this message; it is ill-formed.
			if(p == NULL){
				dbgprint("[NET_AGENT] Signal did not have an END! Discarding...\n");
				continue;
			}

			message msg;
			msg.type = WARN;

			//							v	includes the '\0'
			char* new_text = (char*) kalloc(p - buf, sizeof(char));
			strncpy(new_text, buf + 4, p - buf - 4);

			if(p == buf + 4){
				//There is no text between WARN and END.
				msg.text = NULL;
			}
			else{
				msg.text = new_text;
			}

			messages.push_back(msg);

			dbgprint("[NET_AGENT] Received a warn signal!\n");
			outstanding_warns++;
		}

		if(strncmp("ALERT", buf, 5) == 0){
			char* p = strstr(buf, "END");
			//No END in sight! Abandon this message; it is ill-formed.
			if(p == NULL){
				continue;
			}

			message msg;
			msg.type = ALERT;

			//							v	includes the '\0'
			char* new_text = (char*) kalloc(p - buf, sizeof(char));
			strncpy(new_text, buf + 5, p - buf - 5);

			if(p == buf + 5){
				//There is no text between ALERT and END.
				msg.text = NULL;
			}
			else{
				msg.text = new_text;
			}

			messages.push_back(msg);

			dbgprint("[NET_AGENT] Received an alert signal!\n");
			outstanding_alerts++;
		}

		//Drop any requests for non-localhost at the moment,
		//because doing otherwise could be a security weakness.
		//Remote stuff is NOT supported at the moment,
		//as it requires a bit more forethought.
		//In fact:
		//TODO: change this if we ever support remote stuff.
		//
		//Format of this is:
		//REQ IP [IP] [PORT]
		//See connect_to_requested_ips_listening_place for explanation,
		//because it needs one.
		if(strncmp("REQ IP localhost", buf, 16) == 0){
			message msg;
			msg.type = REQ_IP;

			char ip_str[BUFLEN];
			char port_str[BUFLEN];
			sscanf(buf, "REQ IP %s %s", ip_str, port_str);

			//Fantastic name, I know.
			int r = connect_to_requested_ips_listening_place(ip_str, port_str);
			if(r == -1){
				dbgprint("[NET_AGENT] Received a request for IPs, but server to connect to rejected us!\n");
				continue;
			}

			char* new_text = (char*) kalloc(BUFLEN, sizeof(char));
			snprintf(new_text, BUFLEN, "Connected to %s:%s to serve IPs.", ip_str, port_str);
			msg.text = new_text;
			dbgprint("new_text reads: %s\n", new_text);

			messages.push_back(msg);

			dbgprint("[NET_AGENT] Received a request for IPs!\n");
			outstanding_reqs++;
		}
	}

	return outstanding_infos + outstanding_warns + outstanding_alerts + outstanding_reqs;
}

//Fantastic name, I know.
//Upon a REQ IP from an already-established client,
//(Format is:
//REQ IP [IP] [PORT]
//)
//networking_agent will attempt to connect() to the ip:port combo
//and add it as a client that has_requested_ips.
int networking_agent::connect_to_requested_ips_listening_place(const char* ips_str, const char* port_str){
	int client_sock;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	//Use the loopback address because node is NULL and AI_PASSIVE isn't specified.
	//Cf. man page of getaddrinfo().
	//hints.ai_flags = AI_PASSIVE;
	
	const char* port = port_str;
	
	struct addrinfo* servinfo;
	struct addrinfo* p;
	int r;
	if((r = getaddrinfo(NULL, port, &hints, &servinfo)) != 0){
		dbgprint("[NET_AGENT] getaddrinfo() returned err: %s\n", gai_strerror(r));
		return -1;
	}

	//Loop through all the results and connect to the first we can.
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((client_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			dbgprint("[DEBUG] Couldn't get a socket for some reason.\n");
			continue;
		}

		/*Slightly dead code, but leave it here please.
		//Set to non-blocking.
		if(fcntl(client_sock, F_SETFL, O_NONBLOCK) == -1){
			dbgprint("[NET_RECEIVER] couldn't set socket to non-blocking for some reason.\n");
			return -1;
		}
		*/

		if(connect(client_sock, p->ai_addr, p->ai_addrlen) == -1){
			dbgprint("[NET_AGENT] Couldn't connect() for some reason.\n");
			close(client_sock);
			continue;
		}

		break;
	}

	if(p == NULL){
		dbgprint("[NET_AGENT] Failed to connect.\n");
		return -1;
	}

	//Set to non-blocking.
	/*Done in add_client().
	if(fcntl(client_sock, F_SETFL, O_NONBLOCK) == -1){
		dbgprint("[NET_AGENT] couldn't set socket to non-blocking for some reason.\n");
		close(client_sock);
		return -1;
	}
	*/

	dbgprint("Selected ai_family: %d\nSelected ai_socktype: %d\nSelected ai_protocol: %d\n", p->ai_family, p->ai_socktype, p->ai_protocol);
	char s[INET6_ADDRSTRLEN];
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof(s));
	dbgprint("Connected to: %s", s);

	freeaddrinfo(servinfo);

	//Add this new connection to our clients and set has_requested_ips to true.
	r = add_client(client_sock);
	if(r == -1){
		return -1;
	}
	clients[clients.size() - 1].has_requested_ips = true;

	return 0;
}

//Sends IP addresses to clients with has_requested_ips set to true.
//
//Sent in plaintext according to the following format:
//A connection, with IP version ('4' or '6'), whitespace, followed by two IPs, separated by whitespace:
//[4|6] [SRC] [DST]
//
//Subject to change, obviously. Since we're sending to localhost,
//doing it this way doesn't introduce much in the way of delay.
//And it is simpler to implement.
//
//But it would likely prove problematic if you start sending info across networks,
//or if you start pushing through a lot of connections.
int networking_agent::send_requested_ips(std::vector<conn_entry*>& connections){
	char* buf = (char*) kalloc(BUFLEN, sizeof(char));
	unsigned int multiplier = 1;

	for(unsigned int i = 0; i < clients.size(); i++){
		struct client client = clients[i];

		if(client.has_requested_ips){
			dbgprint("[NET_AGENT] Sending IPs to client %u.\n", i);
			strcpy(buf, "START\n");

			for(conn_entry* entry : connections){
				if(strlen(buf) > (multiplier * BUFLEN) - (3 * INET6_ADDRSTRLEN)){
					multiplier++;
					char* newbuf = (char*) kalloc(BUFLEN * multiplier, sizeof(char));
					strcpy(newbuf, buf);
					free(buf);
					buf = newbuf;
				}
				if(entry->ip_ver == AF_INET){
					strcat(buf, "4 ");
				}
				else{
					strcat(buf, "6 ");
				}

				strcat(buf, entry->local_addr);
				strcat(buf, " ");

				strcat(buf, entry->rem_addr);
				strcat(buf, "\n");
			}

			if(send(client.client_socket, buf, strlen(buf), 0) == -1){
				//Clients are non-blocking -- continue to the next client.
				if(errno == EAGAIN || errno == EWOULDBLOCK){
					dbgprint("[NET_AGENT] Could not send() to client without blocking; skipping...\n");
					break;
				}

				dbgprint("[NET_AGENT] Failed to send IPs to client, closing connection.\n");
				//If for any other reason the client is erroring out,
				//just close the connection.
				rem_client_by_index(i);
				i--;
				break;
			}
		}
	}

	free(buf);

	return 0;
}
