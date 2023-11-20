#include "connections_window.h"

#include "ncurses_funcs.h"

connections_window::connections_window(){
	win = ncurses_funcs::create_newwin((2 * LINES / 3) + 2, COLS, (LINES / 3) - 1, 0);
}

connections_window::~connections_window(){
	delwin(win);
}

#include "debug.h"
void connections_window::draw(){
	wclear(win);
	box(win, 0, 0);

	int height;
	int width;
	getmaxyx(win, height, width);

	mvwprintw(win, 1, 1, "Netid State etc.");
	for(int i = 0; i < (int) connections.size(); i++){
		int y_in_window = i + 2;

		if(y_in_window == height - 1){
			break;
		}

		mvwprintw(win, y_in_window, 1, "%s   ", connections[i]->netid);
		wprintw(win, "%s\t", connections[i]->state);
		wprintw(win, "%s\t", connections[i]->local_addr);
		wprintw(win, "%s", connections[i]->rem_addr);
	}

	wrefresh(win);
}

void connections_window::update_connections(const std::vector<conn_entry*>& connections){
	this->connections = connections;
}

void connections_window::resize(){
	mvwin(win, (LINES/3) - 1, 0);
	wresize(win, (2*LINES/3)+2, COLS);
}
