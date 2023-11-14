#ifndef CONNECTION_ENTRY_H
#define CONNECTION_ENTRY_H

#include <string>

class connection_entry{
	public:
		connection_entry(std::string* temp_input);
		~connection_entry();

		std::string get_protocol();
		std::string get_connection_state();
		std::string get_ip();
		std::string get_process();
	private:
		std::string protocol;
		std::string connection_state;
		std::string ip;
		std::string process;
};

#endif
