#include "window_demon.h"

#include "ncurses_funcs.h"

window_demon::window_demon(){
	bogus_mode = false;
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

	info_win.draw();
	connect_win.draw();
	warn_win.draw();
	if(action_win.is_visible){
		action_win.draw();
	}
	config_win.draw();

	return 0;
}

void window_demon::update_connections(const std::vector<conn_entry*>& connections){
	if(bogus_mode){
		for(conn_entry* entry : bogus_connections){
			free(entry);
		}
		bogus_connections.clear();
		for(int i = 0; i < bogus_entries; i++){
			conn_entry* entry = (conn_entry*) calloc(1, sizeof(conn_entry));

			entry->netid = "BOG";
			entry->state = "JOSHING";
			entry->local_addr = "Me:the past";
			entry->rem_addr = "You:the present";

			bogus_connections.push_back(entry);
		}

		connect_win.update_connections(bogus_connections);
	}
	else{
		connect_win.update_connections(connections);
	}
}

void window_demon::show_actions(){
	action_win.is_visible = true;
}

void window_demon::hide_actions(){
	action_win.is_visible = false;
}

void window_demon::select_right(){
	if(!action_win.is_visible){
		action_win.is_visible = true;
	}
	else{
		;
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

#include "debug.h"
void window_demon::enter_bogus_mode(){
	info_win.print("Entered bogus mode!");
	dbgprint("Entered bogus mode!\n");
	bogus_mode = true;
	bogus_entries = 0;
}

void window_demon::add_bogus_entry(){
	info_win.print("Added bogus entry!");
	bogus_entries++;
}

void window_demon::rem_bogus_entry(){
	info_win.print("Removed bogus entry!");
	if(bogus_entries > 0){
		bogus_entries--;
	}
}
