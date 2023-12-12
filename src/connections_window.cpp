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

#include <sys/socket.h>
#include "options.h"
#include "debug.h"

//Prints one line of the connections_window using conn.
void print_conn_line(WINDOW* win, conn_entry* conn){
	wprintw(win, "%s   ", conn->netid);
	wprintw(win, "%s ", conn->state);

	int y = getcury(win);
	wmove(win, y, 20);

	wprintw(win, "%s      ", conn->read_queue);
	wprintw(win, "%s      ", conn->write_queue);

	char* local_hostname_or_addr;
	char* local_service_or_port;
	char* rem_hostname_or_addr;
	char* rem_service_or_port;

	//The defaults, the raw IP address and port number.
	local_hostname_or_addr = conn->local_addr;
	local_service_or_port = conn->local_port;
	rem_hostname_or_addr = conn->rem_addr;
	rem_service_or_port = conn->rem_port;

	//Assign hostnames/services if enabled and available.
	if(always_show_hostnames){
		dbgprint("[DEBUG] Checking for a hostname...\n");
		if(conn->local_hostname != NULL){
			local_hostname_or_addr = conn->local_hostname;
			dbgprint("[DEBUG] Local has a hostname! It is: %s\n", local_hostname_or_addr);
		}
		if(conn->rem_hostname != NULL){
			rem_hostname_or_addr = conn->rem_hostname;
			dbgprint("[DEBUG] Remote has a hostname! It is: %s\n", rem_hostname_or_addr);
		}
	}
	if(always_show_services){
		if(conn->local_service != NULL){
			local_service_or_port = conn->local_service;
		}
		if(conn->rem_service != NULL){
			rem_service_or_port = conn->rem_service;
		}
	}

	//IPv4
	if(conn->ip_ver == AF_INET){
		wprintw(win, "%s:%s     ", local_hostname_or_addr, local_service_or_port);
		wprintw(win, "%s:%s", rem_hostname_or_addr, rem_service_or_port);
	}
	//IPv6
	else{
		wprintw(win, "[%s]:%s     ", local_hostname_or_addr, local_service_or_port);
		wprintw(win, "[%s]:%s", rem_hostname_or_addr, rem_service_or_port);
	}
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

	mvwprintw(win, 1, 1, "Netid State        Recv-Q Send-Q Local Addr:Port      Rem Addr:Port");

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
	unsigned int max_items_on_shown_page;
	unsigned int starting_entry;
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

	//Hack to find what the length will be after formatting.
	//https://stackoverflow.com/questions/26910479/find-the-length-of-a-formatted-string-in-c
	int len = snprintf(NULL, 0, "%d / %d", shown_page, num_view_pages - 1);
	mvwprintw(win, 1, CONN_WIN_WIDTH - len - 2, "%d / %d", shown_page, num_view_pages - 1);

	int y_offset = 0;
	for(unsigned int i = starting_entry; i < max_items_on_shown_page + starting_entry; i++){
		if(i >= connections.size()){
			break;
		}

		conn_entry* conn = connections[i];

		wmove(win, start_y + y_offset, 1);
		print_conn_line(win, conn);

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
