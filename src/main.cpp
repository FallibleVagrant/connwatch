#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

#include <ncurses.h>

//#include "window_demon.h"
#include "controller_god.h"
#include "debug.h"

FILE* debug_log;

//using std::string;

void init_curses(){
	initscr();
	cbreak();
	noecho();
	curs_set(0);	//Invisible cursor.
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
			fprintf(stderr, "Something broke while updating the windows.\n");
			return -1;
		}

		char input = getch();

		if(input == 'q'){
			return 0;
		}
		else {
			r = god.handle_input(input);
			if(r == -1) {
				dbgprint("Button was pressed that controller_god does not have a handler for.\n");
			}
		}
	}
}

static void int_handler(int i){
	end_curses();
	exit(0);
}

//Cf. note in main.cpp about SIGWINCH.
/*volatile int should_resize = 0;

static void (*ncurses_winch_handler)(int);

static void winch_handler(int i){
	ncurses_winch_handler(i);
	should_resize++;
	mvprintw(0, 0, "WINCHED!");
}*/

#include <signal.h>

//See https://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
static void set_signals(){
	struct sigaction int_action;//, winch_action, old_winch_action;
	int_action.sa_handler = int_handler;
	sigemptyset(&int_action.sa_mask);
	int_action.sa_flags = 0;

	//NOTE: ncurses has its own SIGWINCH handler,
	//which should be called from our own.
	//
	//NOTE: resizing based on SIGWINCH breaks the windows for some reason,
	//taking the lazy approach and resize()ing every draw() for now.
	//
	/*sigaction(SIGWINCH, NULL, &old_winch_action);
	ncurses_winch_handler = old_winch_action.sa_handler;

	winch_action.sa_handler = winch_handler;
	sigemptyset(&winch_action.sa_mask);
	winch_action.sa_flags = 0;*/

	sigaction(SIGINT, &int_action, NULL);
	//sigaction(SIGWINCH, &winch_action, NULL);
}

int main(int argc, char* argv[]){
	if(DEBUG){
		debug_log = fopen("debug.log", "w");
		if(!debug_log){
			fprintf(stderr, "Could not open debug.log...\nExiting...\n");
			return -1;
		}
	}
	dbgprint("Starting...\n");

	init_curses();

	//nodelay(stdscr, true);
	timeout(500);

	controller_god god;

	//Signals.
	set_signals();

	int r = update_loop(god);
	
	end_curses();
	fclose(debug_log);

	return r;
}
