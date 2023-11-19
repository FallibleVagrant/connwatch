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

int window_demon::update(){
	return 0;
}

#include "debug.h"

void window_demon::update_connections(const std::vector<conn_entry*>& connections){
	dbgprint("window_demon is called upon to draw info_win...\n");
	info_win.draw();
	dbgprint("window_demon is called upon to update_connections,\nnum of connections is: %lu.\n", connections.size());
	connect_win.draw(connections);
}
