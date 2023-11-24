#include "action_window.h"

#include "ncurses_funcs.h"

#include <cstring>

#define ACTION_WIN_HEIGHT	(LINES - (LINES / 3) + 1)
#define ACTION_WIN_WIDTH	((COLS / 3) + 1)
#define ACTION_WIN_START_Y	((LINES / 3) - 1)
#define ACTION_WIN_START_X	(COLS - ((COLS / 3) + 1))

action_window::action_window(){
	win = ncurses_funcs::create_newwin(ACTION_WIN_HEIGHT, ACTION_WIN_WIDTH, ACTION_WIN_START_Y, ACTION_WIN_START_X);
	this->is_visible = false;
	choice = 0;
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

#include "debug.h"

void action_window::draw(){
	resize();
	wclear(win);
	box(win, 0, 0);
	mvwprintw(win, 1, 1, "Actions");

	int y;
	int x;
	getmaxyx(win, y, x);

	int start_y = (y / 2) + 1 - (num_choices / 2);
	for(int i = 0; i < num_choices; i++){
		if(i > y - 1){
			break;
		}

		mvwprintw(win, start_y + i, ((COLS/3)/2) - strlen(choices[i])/2, choices[i]);

		if(choice == i){
			mvwchgat(win, start_y + i, 1, (COLS/3) - 1, A_REVERSE, 0, NULL);
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
	mvwin(win, ACTION_WIN_START_Y, ACTION_WIN_START_X);
	wresize(win, ACTION_WIN_HEIGHT, ACTION_WIN_WIDTH);
}

const char* action_window::get_selection(){
	return choices[this->choice];
}
