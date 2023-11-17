#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

#include <ncurses.h>

//#include "window_demon.h"
#include "controller_god.h"

using std::string;

void init_curses(){
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	refresh();
}

void end_curses(){
	endwin();
}

/*
void update_curses(window_demon& demon){
	//printw("Hello, World.\n");
}
*/

int update_loop(controller_god& god){
	while(true){
		int r = god.update();
		if(r == -1){
			fprintf(stderr, "Something broke\n");
			return -1;
		}

		char input = getch();

		if(input == 'q'){
			return 0;
		}
		else {
			r = god.handle_input(input);
			if(r == -1) {
				fprintf(stderr, "Button was pressed that controller_god does not have a handler for.\n");
			}
		}
	}
}

int main(int argc, char* argv[]){

	init_curses();

	//nodelay(stdscr, true);
	timeout(500);

	controller_god god;

	int r = update_loop(god);
	
	end_curses();

	return r;
}
