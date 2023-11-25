#include "window_demon.h"

#include "ncurses_funcs.h"

#include <cstring>

window_demon::window_demon(){}

void window_demon::start(model_angel* ap){
	angel_pointer = ap;

	connect_win.start(ap);
	warn_win.start(ap);
}

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

	if(angel_pointer->get_num_connections() == 0){
		action_win.is_visible = false;
	}

	info_win.draw();
	connect_win.draw();
	warn_win.draw();
	if(action_win.is_visible){
		action_win.draw();
	}
	config_win.draw();

	return 0;
}

void window_demon::resolve_action(const char* selection){
	if(strcmp(selection, "Hello") == 0){
		info_win.print("Hello, World.");
	}
	else if(strcmp(selection, "Dig") == 0){
		//Etc.
		;
	}
}

void window_demon::select_right(){
	if(angel_pointer->get_num_connections() > 0){
		if(!action_win.is_visible){
			action_win.is_visible = true;
		}
		else{
			this->resolve_action(action_win.get_selection());
		}
	}
}

void window_demon::select_left(){
	if(action_win.is_visible){
		action_win.is_visible = false;
	}
	else{
		;
	}
}

void window_demon::select_down(){
	if(action_win.is_visible){
		action_win.select_down();
	}
	else{
		connect_win.select_down();
	}
}

void window_demon::select_up(){
	if(action_win.is_visible){
		action_win.select_up();
	}
	else{
		connect_win.select_up();
	}
}

void window_demon::trigger_resize(){
	info_win.resize();
	connect_win.resize();
	warn_win.resize();
	action_win.resize();
	config_win.resize();
}
