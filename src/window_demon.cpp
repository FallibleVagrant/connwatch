#include "window_demon.h"

#include "ncurses_funcs.h"

#include <cstring>

window_demon::window_demon(){}

int window_demon::start(model_angel* ap){
	angel_pointer = ap;

	connect_win.start(ap);
	warn_win.start(ap);
	alarum_win.start(ap);
	log_win.start(ap);

	return 0;
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

enum{
	CONNS,
	LOG,
};

bool main_window_bitmask[] = {
	[CONNS] = true,
	[LOG] = false,
};

unsigned int bitmask_len = 2;

/*
void window_demon::show_main_window(main_window_type type){
	for(unsigned int i = 0; i < bitmask_len; i++){
		main_window_bitmask[i] = false;
	}
	main_window_bitmask[type] = true;
}
*/

void window_demon::show_connections(){
	//show_main_window(CONNS);
	for(unsigned int i = 0; i < bitmask_len; i++){
		main_window_bitmask[i] = false;
	}
	main_window_bitmask[CONNS] = true;
}

void window_demon::show_log(){
	//show_main_window(LOG);
	for(unsigned int i = 0; i < bitmask_len; i++){
		main_window_bitmask[i] = false;
	}
	main_window_bitmask[LOG] = true;
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

	if(main_window_bitmask[CONNS]){
		connect_win.draw();
	}
	if(main_window_bitmask[LOG]){
		log_win.draw();
	}

	warn_win.draw();

	if(action_win.is_visible){
		action_win.draw();
	}

	config_win.draw();

	if(alarum_win.is_animating()){
		alarum_win.draw();
	}

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
		if(main_window_bitmask[CONNS]){
			connect_win.select_down();
		}
		if(main_window_bitmask[LOG]){
			log_win.select_down();
		}
	}
}

void window_demon::select_up(){
	if(action_win.is_visible){
		action_win.select_up();
	}
	else{
		if(main_window_bitmask[CONNS]){
			connect_win.select_up();
		}
		if(main_window_bitmask[LOG]){
			log_win.select_up();
		}
	}
}

void window_demon::trigger_resize(){
	info_win.resize();
	connect_win.resize();
	warn_win.resize();
	action_win.resize();
	config_win.resize();
	alarum_win.resize();
	log_win.resize();
}

#include "common.h"

#include "debug.h"
void window_demon::trigger_alarum_popup(const char* alarum_message){
	if(alarum_message == NULL){
		alarum_win.print("Default Alarm message.");
	}
	else{
		dbgprint("[WINDOW_DEMON] Received alarm with message: %s\n", alarum_message);
		alarum_win.print("%s", alarum_message);
	}
	alarum_win.start_animating(ticker);
}

void window_demon::toggle_hostnames(){
	config_win.toggle_hostnames();
}

void window_demon::toggle_services(){
	config_win.toggle_services();
}

void window_demon::toggle_sound(){
	config_win.toggle_sound();
}

void window_demon::cycle_sort_method(){
	config_win.cycle_sort_method();
}
