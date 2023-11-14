#include "window_demon.h"

#include "ncurses_funcs.h"

window_demon::window_demon(){}

window_demon::~window_demon(){
	//Destructors are called regardless.
	//info_win.~info_window();
	//connect_win.~connections_window();
	//action_win.~action_window();

	/*Also unnecesary -- destructors are called.
	for(connection_entry* entry : connections){
		delete entry;
	}
	*/
}

void window_demon::update_connections(const std::vector<std::string*>& new_connections){
	for(connection_entry* entry : connections){
		delete entry;
	}
	connections.clear();

	for(std::string* new_connection : new_connections){
		if(new_connection != NULL){
			connections.push_back(new connection_entry(new_connection));
		}
	}

	connect_win.show_connections(connections);
	action_win.show_actions(connections.size());
}
