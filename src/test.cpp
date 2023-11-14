#include <iostream>
#include <vector>
#include <string>

#include "ncurses_funcs.h"
#include "connections_window.h"
#include "connection_entry.h"
#include "action_window.h"

int main(int argc, char* argv[]){
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	refresh();
	WINDOW* win = ncurses_funcs::create_newwin(10, 20, 0, 0);
	//box(stdscr, 0, 0);

	mvwprintw(win, 3, 3, "hello");

	wrefresh(win);
	//refresh();

	connections_window test_window = connections_window();
	std::vector<connection_entry*> connection_entries;

	connection_entries.push_back(new connection_entry(new std::string("This is connection 0")));
	connection_entries.push_back(new connection_entry(new std::string("This is connection 1")));
	connection_entries.push_back(new connection_entry(new std::string("This is connection 2")));
	connection_entries.push_back(new connection_entry(new std::string("This is connection 3")));

	test_window.show_connections(connection_entries);

	action_window second_test_window = action_window();
	second_test_window.show_actions(4);

	getch();

	endwin();

	return 0;
}
