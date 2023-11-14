#include "connection_entry.h"

#include <cassert>
#include <regex>

//Parse raw_input into actual values.
connection_entry::connection_entry(std::string* raw_input){
	if(raw_input == NULL){
		//throw std::invalid_argument("Pointer to NULL input as connection!\n");
	}

	//Protocol
	if(raw_input->find("tcp") != std::string::npos){
		protocol = "TCP";
	}
	else{
		protocol = "UDP";
	}

	//connection_state
	if(raw_input->find("UNCONN") != std::string::npos){
		connection_state = "UNCONN";
	}
	if(raw_input->find("LISTEN") != std::string::npos){
		connection_state = "LISTEN";
	}
	if(raw_input->find("ESTAB") != std::string::npos){
		connection_state = "ESTAB";
	}

	//TODO: Add for ip
	//Doesn't check if conforms to ipv4 format, just existence of something vaguely similar.
	std::regex ipv4_regex("([0-9]+\\.){3}[0-9]+");
	std::smatch ipv4_matches;

	//Doesn't check if conforms to ipv6 format, just existence of something vaguely similar.
	std::regex ipv6_regex("([a-fA-F0-9]{0,}:){6}[a-fA-F0-9]{0,}");
	std::smatch ipv6_matches;

	bool is_ipv4 = false;
	bool is_ipv6 = false;

	is_ipv4 = std::regex_search(*raw_input, ipv4_matches, ipv4_regex);
	is_ipv6 = std::regex_search(*raw_input, ipv6_matches, ipv6_regex);

	if(!is_ipv4 && !is_ipv6){
		//throw std::invalid_argument...TODO
	}

	//Cannot be both or neither.
	//assert(is_ipv4 != is_ipv6);

	if(is_ipv4){
		ip = "ipv4";
	}
	if(is_ipv6){
		ip = "ipv6";
	}
}

connection_entry::~connection_entry(){

}

std::string connection_entry::get_protocol(){
	return protocol;
}

std::string connection_entry::get_connection_state(){
	return connection_state;
}

std::string connection_entry::get_ip(){
	return ip;
}

std::string connection_entry::get_process(){
	return process;
}
