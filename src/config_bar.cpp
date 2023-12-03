#include "config_bar.h"

#include "ncurses_funcs.h"

#include <cstring>

#include "colors.h"
#include "options.h"

#define CONFIG_WIN_HEIGHT	1
#define CONFIG_WIN_WIDTH	COLS
#define CONFIG_WIN_START_Y	(LINES - 1)
#define CONFIG_WIN_START_X	0

config_bar::config_bar(){
	win = ncurses_funcs::create_newwin(CONFIG_WIN_HEIGHT, CONFIG_WIN_WIDTH, CONFIG_WIN_START_Y, CONFIG_WIN_START_X);
}

config_bar::~config_bar(){
	delwin(win);
}

void config_bar::draw(){
	resize();
	wclear(win);

	wattron(win, A_ALTCHARSET);
	wprintw(win, "mq");
	wattroff(win, A_ALTCHARSET);

	wprintw(win, "[");
	wattron(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "s");
	wattroff(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "]ort-by: ");

	switch(sort_method){
		case SORT_NONE:
			wprintw(win, "none");
			break;
		case SORT_NETID_ASC:
			wprintw(win, "netid asc");
			break;
		case SORT_NETID_DESC:
			wprintw(win, "netid desc");
			break;
		case SORT_STATE_ASC:
			wprintw(win, "state asc");
			break;
		case SORT_STATE_DESC:
			wprintw(win, "state desc");
			break;
		default:
			wprintw(win, "err");
			break;
	}

	wprintw(win, " | ");

	wprintw(win, "[");
	wattron(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "S");
	wattroff(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "]ound: ");

	if(sound_enabled){
		wprintw(win, "on");
	}
	else{
		wprintw(win, "no");
	}

	wprintw(win, " | ");

	wprintw(win, "host[");
	wattron(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "n");
	wattroff(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "]ames: ");

	if(always_show_hostnames){
		wprintw(win, "on");
	}
	else{
		wprintw(win, "no");
	}

	wprintw(win, " | ");

	wprintw(win, "ser[");
	wattron(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "v");
	wattroff(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "]ices: ");

	if(always_show_services){
		wprintw(win, "on");
	}
	else{
		wprintw(win, "no");
	}

	int x = getcurx(win);
	wattron(win, A_ALTCHARSET);
	for(int i = x; i < CONFIG_WIN_WIDTH - 1; i++){
		wprintw(win, "q");
	}
	wprintw(win, "j");
	wattroff(win, A_ALTCHARSET);

	wrefresh(win);
}

void config_bar::resize(){
	mvwin(win, CONFIG_WIN_START_Y, CONFIG_WIN_START_X);
	wresize(win, CONFIG_WIN_HEIGHT, CONFIG_WIN_WIDTH);
}

void config_bar::toggle_hostnames(){
	always_show_hostnames = !always_show_hostnames;
	always_resolve_hostnames = !always_resolve_hostnames;
}

void config_bar::toggle_services(){
	always_show_services = !always_show_services;
	always_resolve_services = !always_resolve_services;
}

void config_bar::toggle_sound(){
	sound_enabled = !sound_enabled;
}

void config_bar::cycle_sort_method(){
	sort_method++;
	if(sort_method >= num_sort_methods){
		sort_method = 0;
	}
}
