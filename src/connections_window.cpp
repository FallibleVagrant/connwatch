#include "connections_window.h"

#include "ncurses_funcs.h"

connections_window::connections_window(){
	win = ncurses_funcs::create_newwin((2 * LINES / 3) + 2, 2 * COLS / 3, (LINES / 3) - 1, 0);
}

connections_window::~connections_window(){
	delwin(win);
}

#include "debug.h"

void connections_window::draw(const std::vector<conn_entry*>& connections){
	int height;
	int width;
	getmaxyx(win, height, width);

	dbgprint("Drawing connections_window...\n");
	dbgprint("Connections is size: %lu\n", connections.size());
	for(int i = 0; i < (int) connections.size(); i++){
		int y_in_window = i + 1;

		if(y_in_window == height - 1){
			break;
		}

		mvwprintw(win, y_in_window, 1, "%s\t", connections[i]->netid);
		wprintw(win, "%s\t", connections[i]->state);
		wprintw(win, "%s\t", connections[i]->local_addr);
		wprintw(win, "%s", connections[i]->rem_addr);
	}

	wrefresh(win);
}
