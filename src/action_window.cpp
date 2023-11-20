#include "action_window.h"

#include "ncurses_funcs.h"

#include <cstring>

action_window::action_window(){
	win = ncurses_funcs::create_newwin((2*LINES/3)+2, (COLS/3)+1, (LINES/3)-1, COLS - ((COLS/3)+1));
	this->is_visible = false;
}

action_window::~action_window(){
	delwin(win);
}

static const char* choices[] = {
	"Hello",
	"Next",
	"Unknown",
	"Log",
	"Dig",
	"Where"
};

static int num_choices = sizeof(choices) / sizeof(choices[0]);
static int choice = 0;

#include "debug.h"

void action_window::draw(){
	resize();
	wclear(win);
	box(win, 0, 0);
	mvwprintw(win, 1, 1, "Actions");

	int y;
	int x;
	getmaxyx(win, y, x);

	int y_in_window = (y / 2) + 1 - (num_choices / 2);
	for(int i = 0; i < num_choices; i++){
		if(i > y - 1){
			break;
		}

		mvwprintw(win, y_in_window + i, ((COLS/3)/2) - strlen(choices[i])/2, choices[i]);
		if(choice == i){
			mvwchgat(win, y_in_window + i, 1, (COLS/3) - 1, A_REVERSE, 0, NULL);
		}
	}

	wrefresh(win);
}

void action_window::select_down(){
	choice++;
	choice %= num_choices;
}

void action_window::select_up(){
	choice--;
	if(choice < 0){
		choice = num_choices - 1;
	}
}

void action_window::resize(){
	mvwin(win, (LINES/3)-1, COLS - ((COLS/3)+1));
	wresize(win, (2*LINES/3)+2, (COLS/3)+1);
}
