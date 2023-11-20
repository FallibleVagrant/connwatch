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

void window_demon::update_connections(const std::vector<conn_entry*>& connections){
	info_win.draw();
	connect_win.draw(connections);
	if(action_win.is_visible){
		action_win.draw();
	}
}

void window_demon::show_actions(){
	action_win.is_visible = true;
}

void window_demon::hide_actions(){
	action_win.is_visible = false;
}


void window_demon::select_down(){
	if(action_win.is_visible){
		action_win.select_down();
	}
	else{
		//connect_win
		;
	}
}

void window_demon::select_up(){
	if(action_win.is_visible){
		action_win.select_up();
	}
	else{
		//connect_win
		;
	}
}
