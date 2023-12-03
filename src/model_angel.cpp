#include "model_angel.h"

#include "debug.h"
#include "window_demon.h"
#include "net_common.h"

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

#define MSG_BUFLEN 30

void model_angel::add_message_to_log(int type, char* text){
	message* p = (message*) calloc(1, sizeof(message));

	p->type = type;
	//This was already allocated on the heap by the networking agent.
	p->text = text;

	message_log.insert(message_log.begin(), p);
}

int model_angel::check_networking_agent(){
	int r = net_agent.check_for_incoming_connections();
	if(r == -1){
		return -1;
	}

	message messages[MSG_BUFLEN];

	int num_messages = net_agent.check_for_messages(messages, MSG_BUFLEN);
	if(num_messages == -1){
		return -1;
	}

	for(unsigned int i = 0; i < num_messages; i++){
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
			default:
				dbgprint("[MODEL_ANGEL] Message with unrecognised type somehow got through the filters!\n");
				break;
		}
	}

	return 0;
}

#include "common.h"

int model_angel::update(){
	int r;
	if(ticker % 2 == 1){
		//Deallocate and clear.
		for(conn_entry* entry : connections){
			free_conn_entry(entry);
		}
		connections.clear();

		r = proc_read.fetch_connections(connections);
		if(r == -1){
			dbgprint("Could not fetch_connections() from system.\n");
			return -1;
		}
	}

	r = check_networking_agent();
	if(r == -1){
		dbgprint("Could not communicate with networking agent.\n");
		return -1;
	}

	return 0;
}
