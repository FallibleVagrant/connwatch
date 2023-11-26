#include "alarum_window.h"

#include "ncurses_funcs.h"

#include "model_angel.h"

#define ALARUM_WIN_HEIGHT	(LINES / 3)
#define ALARUM_WIN_WIDTH	((COLS / 3) + 1)
#define ALARUM_WIN_START_Y	((LINES / 3) - 1)
#define ALARUM_WIN_START_X	((COLS / 3) - 1)

alarum_window::alarum_window(){
	win = ncurses_funcs::create_newwin(ALARUM_WIN_HEIGHT, ALARUM_WIN_WIDTH, ALARUM_WIN_START_Y, ALARUM_WIN_START_X);
	initial_ticker = 0;
	is_playing = false;
}

void alarum_window::start(model_angel* ap){
	this->angel_pointer = ap;
}

alarum_window::~alarum_window(){
	delwin(win);
}

#include "common.h"

void alarum_window::draw(){
	//Ticker has overflowed! Just reset the animation.
	if(ticker < initial_ticker){
		initial_ticker = ticker;
	}
	resize();
	wclear(win);
	box(win, 0, 0);

	mvwprintw(win, 1, 1, "Alert triggered! %d", ticker - initial_ticker);

	wrefresh(win);

	if(ticker - initial_ticker >= 10){
		is_playing = false;
	}
}

void alarum_window::resize(){
	mvwin(win, ALARUM_WIN_START_Y, ALARUM_WIN_START_X);
	wresize(win, ALARUM_WIN_HEIGHT, ALARUM_WIN_WIDTH);
}

void alarum_window::start_animating(unsigned int init_ticker){
	is_playing = true;
	initial_ticker = init_ticker;
}

bool alarum_window::is_animating(){
	return is_playing;
}
