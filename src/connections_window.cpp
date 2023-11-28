#include "connections_window.h"

#include "ncurses_funcs.h"

#define CONN_WIN_HEIGHT		(LINES - (LINES / 3) + 1)
#define CONN_WIN_WIDTH		COLS
#define CONN_WIN_START_Y	((LINES / 3) - 1)
#define CONN_WIN_START_X	0

#define MAX_CONNS_PER_PAGE	(CONN_WIN_HEIGHT - 3)

connections_window::connections_window(){
	win = ncurses_funcs::create_newwin(CONN_WIN_HEIGHT, CONN_WIN_WIDTH, CONN_WIN_START_Y, CONN_WIN_START_X);
	angel_pointer = NULL;
	choice = 0;
}

void connections_window::start(model_angel* ap){
	angel_pointer = ap;
}

connections_window::~connections_window(){
	delwin(win);
}

#include "debug.h"
#include "main_window_header.h"
#include "colors.h"
#include "scrolling_machine.h"

void connections_window::draw(){
	resize();
	wclear(win);
	box(win, 0, 0);

	print_header(win, "Connections");

	mvwprintw(win, 1, 1, "Netid State etc.");

	std::vector<conn_entry*> connections = angel_pointer->get_connections();

	if(connections.size() == 0){
		wrefresh(win);
		return;
	}
	if(choice >= connections.size()){
		choice = connections.size() - 1;
	}

	int height;
	int width;
	getmaxyx(win, height, width);

	int shown_page;
	int num_view_pages;
	int max_items_on_shown_page;
	int starting_entry;
	int shown_window_choice;

	compute_range_of_page(CONN_WIN_HEIGHT - 3, connections.size(), choice, &shown_page, &num_view_pages, &max_items_on_shown_page, &starting_entry, &shown_window_choice);

	int start_y = 2;

	if(shown_page > 0){
		mvwprintw(win, start_y, COLS/2, "^^^");
		start_y = 3;
	}

	//If more than one page and not on last page...
	if(num_view_pages > 1 && shown_page < num_view_pages - 1){
		mvwprintw(win, start_y + max_items_on_shown_page, COLS/2, "vvv");
	}

	mvwprintw(win, 1, 20, "%d / %d", shown_page, num_view_pages - 1);

	int y_offset = 0;
	for(int i = starting_entry; i < max_items_on_shown_page + starting_entry; i++){
		if(i >= connections.size()){
			break;
		}

		mvwprintw(win, start_y + y_offset, 1, "%s   ", connections[i]->netid);
		wprintw(win, "%s\t", connections[i]->state);
		wprintw(win, "%s\t", connections[i]->local_addr);
		wprintw(win, "%s", connections[i]->rem_addr);

		if(shown_window_choice == y_offset){
			mvwchgat(win, start_y + y_offset, 1, COLS-2, A_REVERSE, 0, NULL);
		}

		y_offset++;
	}

	wrefresh(win);
}

void connections_window::select_down(){
	std::vector<conn_entry*> connections = angel_pointer->get_connections();
	if(connections.size() > 0){
		choice++;
		choice %= connections.size();
	}
}

void connections_window::select_up(){
	std::vector<conn_entry*> connections = angel_pointer->get_connections();
	if(connections.size() > 0){
		choice--;
		if(choice < 0){
			choice = connections.size() - 1;
		}
	}
}

void connections_window::resize(){
	mvwin(win, CONN_WIN_START_Y, CONN_WIN_START_X);
	wresize(win, CONN_WIN_HEIGHT, CONN_WIN_WIDTH);
}
