#include "model_angel.h"

#include "debug.h"
#include "window_demon.h"
#include "net_common.h"
#include "common.h"

model_angel::model_angel(){
	num_warnings = 0;
	num_alerts = 0;
}

int model_angel::start(window_demon* pointer_to_demon){
	demon_pointer = pointer_to_demon;

	int r = net_agent.init();
	if(r == -1){
		dbgprint("Failed to start networking agent!\n");
		return -1;
	}

	r = proc_read.init();
	if(r == -1){
		dbgprint("Failed to start /proc reading facilities!\n");
		return -1;
	}

	return 0;
}

model_angel::~model_angel(){
	for(message* p : message_log){
		free(p->text);
		free(p);
	}
}

void model_angel::warn(){
	this->num_warnings++;
}

void model_angel::alert(const char* alarum_message){
	if(this->num_alerts == 0){
		demon_pointer->trigger_alarum_popup(alarum_message);
	}
	this->num_alerts++;
}

void model_angel::reset_alerts_and_warns(){
	this->num_warnings = 0;
	this->num_alerts = 0;
}

std::vector<conn_entry*> model_angel::get_connections(){
	return this->connections;
}

std::vector<message*> model_angel::get_log(){
	return this->message_log;
}

unsigned int model_angel::get_num_connections(){
	return this->connections.size();
}

int model_angel::get_num_warnings(){
	return this->num_warnings;
}

int model_angel::get_num_alerts(){
	return this->num_alerts;
}

void model_angel::add_message_to_log(int type, char* text){
	message* p = (message*) kalloc(1, sizeof(message));

	p->type = type;
	//This was already allocated on the heap by the networking agent.
	p->text = text;

	message_log.insert(message_log.begin(), p);
}

#include <string.h>

int model_angel::check_networking_agent(){
	std::vector<char*> added_connection_msgs;
	int r = net_agent.check_for_incoming_connections(added_connection_msgs);
	if(r == -1){
		return -1;
	}

	//Made a connection.
	if(r > 0){
		for(char* msg : added_connection_msgs){
			this->add_message_to_log(INFO, msg);
		}
	}

	std::vector<message> messages;

	r = net_agent.check_for_messages(messages);
	if(r == -1){
		return -1;
	}

	for(unsigned int i = 0; i < messages.size(); i++){
		if(messages[i].text != NULL){
			dbgprint("[MODEL_ANGEL] Received a message! Text reads: %s\n", messages[i].text);
		}
		switch(messages[i].type){
			case INFO:
				//this->memo();
				this->add_message_to_log(INFO, messages[i].text);
				break;
			case WARN:
				this->warn();
				this->add_message_to_log(WARN, messages[i].text);
				break;
			case ALERT:
				this->alert(messages[i].text);
				this->add_message_to_log(ALERT, messages[i].text);
				break;
			case REQ_IP:
				this->add_message_to_log(REQ_IP, NULL);
				break;
			default:
				dbgprint("[MODEL_ANGEL] Message with unrecognised type somehow got through the filters!\n");
				break;
		}
	}

	return 0;
}

#include "options.h"
#include <string.h>
#include <algorithm>

//Thank you to:
//https://stackoverflow.com/questions/2758080/how-to-sort-an-stl-vector
//for example code.
void model_angel::sort_connections(){
	switch(sort_method){
		case SORT_NONE:
			;
			break;
		case SORT_NETID_ASC:
			std::sort(connections.begin(), connections.end(), [] (conn_entry* a, conn_entry* b) { return strcmp(a->netid, b->netid) < 0; });
			break;
		case SORT_NETID_DESC:
			std::sort(connections.begin(), connections.end(), [] (conn_entry* a, conn_entry* b) { return strcmp(a->netid, b->netid) > 0; });
			break;
		case SORT_STATE_ASC:
			std::sort(connections.begin(), connections.end(), [] (conn_entry* a, conn_entry* b) { return strcmp(a->state, b->state) < 0; });
			break;
		case SORT_STATE_DESC:
			std::sort(connections.begin(), connections.end(), [] (conn_entry* a, conn_entry* b) { return strcmp(a->state, b->state) > 0; });
			break;
		default:
			dbgprint("[MODEL_ANGEL] Attempted to sort by an unknown sort method!\n");
			break;
	}
}

#include "common.h"

int model_angel::update(){
	int r;
	if(time(NULL) > time_since_last_connection_update + time_between_connection_updates
			|| time_since_last_connection_update == 0){
		time_since_last_connection_update = time(NULL);
		//Deallocate and clear.
		for(conn_entry* entry : connections){
			free_conn_entry(entry);
		}
		connections.clear();

		dbgprint("[MODEL_ANGEL] fetch_connections()...\n");

		r = proc_read.fetch_connections(connections);
		if(r == -1){
			dbgprint("[MODEL_ANGEL] Could not fetch_connections() from system.\n");
			return -1;
		}

		this->sort_connections();
	}

	if(time(NULL) > time_since_last_ip_reqs_update + time_between_ip_reqs_updates){
		time_since_last_ip_reqs_update = time(NULL);
		dbgprint("[MODEL_ANGEL] send_requested_ips()...\n");
		r = net_agent.send_requested_ips(connections);
		if(r == -1){
			dbgprint("[MODEL_ANGEL] Could not send connection info to subscribed process(es)");
			return -1;
		}
	}

	dbgprint("[MODEL_ANGEL] check_networking_agent()...\n");
	r = check_networking_agent();
	if(r == -1){
		dbgprint("[MODEL_ANGEL] Could not communicate with networking agent.\n");
		return -1;
	}

	return 0;
}
