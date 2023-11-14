#include "connections_window.h"

#include "ncurses_funcs.h"

connections_window::connections_window(){
	win = ncurses_funcs::create_newwin((2 * LINES / 3) + 2, 2 * COLS / 3, (LINES / 3) - 1, 0);
}

connections_window::~connections_window(){
	delwin(win);
}

void connections_window::show_connections(const std::vector<connection_entry*> connections){
	int height;
	int width;
	getmaxyx(win, height, width);

	for(int i = 0; i < (int) connections.size(); i++){
		int y_in_window = i + 1;

		if(y_in_window == height - 1){
			break;
		}

		mvwprintw(win, y_in_window, 1, "%s\t", connections[i]->get_protocol().c_str());
		wprintw(win, "%s\t", connections[i]->get_connection_state().c_str());
		wprintw(win, "%s\t", connections[i]->get_ip().c_str());
		wprintw(win, "%s", connections[i]->get_process().c_str());
	}

	wrefresh(win);
}
