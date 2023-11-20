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
	//After receiving SIGWINCH, resize demon.
	//Cf. note in main.cpp about SIGWINCH.
	/*if(should_resize){
		this->trigger_resize();
		should_resize = 0;
	}*/

	info_win.draw();
	connect_win.draw();
	if(action_win.is_visible){
		action_win.draw();
	}
	config_win.draw();

	return 0;
}

void window_demon::update_connections(const std::vector<conn_entry*>& connections){
	connect_win.update_connections(connections);
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

void window_demon::trigger_resize(){
	info_win.resize();
	connect_win.resize();
	action_win.resize();
	config_win.resize();
}
