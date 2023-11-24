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

void connections_window::draw(){
	resize();
	wclear(win);
	box(win, 0, 0);

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

	/*
	Behold! The Scrolling Machine!
	Braveness upon you if you are here to debug it.
	Each connection page holds a variable number of connections,
	which makes computing which page you are on non-trivial.

	The first page, for example, may have MAX_CONNS_PER_PAGE connections, or
	MAX_CONNS_PER_PAGE - 1 connections, because the little "vvv" takes up
	the space of one connection.

	Middle pages (not the first or last page.) always take up MAX_CONNS_PER_PAGE - 2 connections.
	This is because they have both the "^^^" and the "vvv", taking up two spaces.

	The last page takes up between two and MAX_CONNS_PER_PAGE - 1 connections.
	A last page cannot have just one connection, as it would instead be used to fill the previous page.

	The point is to derive everything from this->choice, to avoid state and also because I wanted the challenge of implementing this.
	*/

	int connection_page;
	int shown_window_choice;
	int max_conns_on_this_page;
	int num_connection_pages;

	//One connection page exists.
	if(connections.size() <= MAX_CONNS_PER_PAGE){
		num_connection_pages = 1;

		shown_window_choice = choice;
		connection_page = 0;
		max_conns_on_this_page = MAX_CONNS_PER_PAGE;
	}
	else{
		//Two connection pages exist, but not more.
		//(connections.size() > MAX_CONNS_PER_PAGE)
		if(connections.size() <= (2 * (MAX_CONNS_PER_PAGE - 1))){
			num_connection_pages = 2;
			//Still on the first connection page.
			if(choice < MAX_CONNS_PER_PAGE - 1){
				shown_window_choice = choice;
				connection_page = 0;
				max_conns_on_this_page = MAX_CONNS_PER_PAGE - 1;
			}
			else{
				//On the second (and last) connection page.
				//(choice > MAX_CONNS_PER_PAGE - 1)
				connection_page = 1;
				shown_window_choice = choice - (MAX_CONNS_PER_PAGE - 1);
				max_conns_on_this_page = MAX_CONNS_PER_PAGE - 1;
			}
		}
		//More than two connection pages exist.
		//(connections.size() > MAX_CONNS_PER_PAGE)
		//					&&
		//(connections.size() > (2 * (MAX_CONNS_PER_PAGE - 1)))
		else{
			int num_conns_excl_first_page = connections.size() - (MAX_CONNS_PER_PAGE - 1);
			int num_middle_pages = num_conns_excl_first_page / (MAX_CONNS_PER_PAGE - 2);

			//If the last page is filled, or (filled - 1), it was accidentally counted in num_middle_pages.
			//NOTE: the last page can never have only one connection, as it would instead
			//be appended to the page before it.
			if(num_conns_excl_first_page % (MAX_CONNS_PER_PAGE - 2) == 1
					|| num_conns_excl_first_page % (MAX_CONNS_PER_PAGE - 2) == 0){
				num_middle_pages--;
			}

			num_connection_pages = num_middle_pages + 2;

			//Still on the first connection page.
			if(choice <= MAX_CONNS_PER_PAGE - 1){
				shown_window_choice = choice;
				connection_page = 0;
				max_conns_on_this_page = MAX_CONNS_PER_PAGE - 1;
			}
			else{
				int num_conns_before_last_page = (MAX_CONNS_PER_PAGE - 1) + (num_middle_pages * (MAX_CONNS_PER_PAGE - 2));

				//Past the first connection page, but not on the last.
				//(choice > MAX_CONNS_PER_PAGE - 1)
				if(choice <= num_conns_before_last_page){
					//We know we are not on the last page, so this calc will never count the last page.
					int num_middle_pages_passed = (choice - (MAX_CONNS_PER_PAGE - 1)) / (MAX_CONNS_PER_PAGE - 2);

					connection_page = num_middle_pages_passed + 1;
					shown_window_choice = (choice - (MAX_CONNS_PER_PAGE - 1)) % (MAX_CONNS_PER_PAGE - 2);
					max_conns_on_this_page = MAX_CONNS_PER_PAGE - 2;
				}
				//On the last connection page.
				//(choice > MAX_CONNS_PER_PAGE - 1)
				//				&&
				//(choice > num_conns_before_last_page)
				else{
					shown_window_choice = choice - num_conns_before_last_page;
					connection_page = 1 + num_middle_pages;
					max_conns_on_this_page = MAX_CONNS_PER_PAGE - 1;
				}
			}
		}
	}

	int start_y = 2;

	if(connection_page > 0){
		mvwprintw(win, start_y, COLS/2, "^^^");
		start_y = 3;
	}

	//If more than one page and not on last page...
	if(connections.size() > MAX_CONNS_PER_PAGE && connection_page < num_connection_pages - 1){
		mvwprintw(win, start_y + max_conns_on_this_page, COLS/2, "vvv");
	}

	mvwprintw(win, 1, 20, "%d / %d", connection_page, num_connection_pages - 1);

	for(int i = 0; i < max_conns_on_this_page; i++){
		int conn_idx;
		if(connection_page == 0){
			conn_idx = i;
		}
		if(connection_page >= 1){
			int num_middle_pages_passed = connection_page - 1;
			conn_idx = (MAX_CONNS_PER_PAGE - 1) + (num_middle_pages_passed * (MAX_CONNS_PER_PAGE - 2)) + i;
		}
		if(conn_idx >= connections.size()){
			break;
		}

		mvwprintw(win, start_y + i, 1, "%s   ", connections[conn_idx]->netid);
		wprintw(win, "%s\t", connections[conn_idx]->state);
		wprintw(win, "%s\t", connections[conn_idx]->local_addr);
		wprintw(win, "%s", connections[conn_idx]->rem_addr);

		if(shown_window_choice == i){
			mvwchgat(win, start_y + i, 1, COLS-2, A_REVERSE, 0, NULL);
		}
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
