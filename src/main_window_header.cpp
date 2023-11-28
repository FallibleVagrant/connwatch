#include "main_window_header.h"

#include "colors.h"
#include <cstring>

#define SPACE_BETWEEN 3

void print_header(WINDOW* win, const char* win_name){
	//connections
	if(strcmp(win_name, "Connections") == 0){
		//wattron(win, A_UNDERLINE);
	}
	mvwprintw(win, 0, 2, "[");
	wattron(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "c");
	wattroff(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "]onnections");
	if(strcmp(win_name, "Connections") == 0){
		//wattroff(win, A_UNDERLINE);
	}

	int x;
	x = getcurx(win);
	wmove(win, 0, x + SPACE_BETWEEN);

	//log
	if(strcmp(win_name, "Log") == 0){
		//wattron(win, A_UNDERLINE);
	}
	wprintw(win, "l[");
	wattron(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "o");
	wattroff(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "]g");
	if(strcmp(win_name, "Log") == 0){
		//wattroff(win, A_UNDERLINE);
	}

	x = getcurx(win);
	wmove(win, 0, x + SPACE_BETWEEN);

	//output
	if(strcmp(win_name, "Output") == 0){
		//wattron(win, A_UNDERLINE);
	}
	wprintw(win, "o[");
	wattron(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "u");
	wattroff(win, COLOR_PAIR(ORANGE_AND_BLACK));
	wprintw(win, "]tput");
	if(strcmp(win_name, "Output") == 0){
		//wattroff(win, A_UNDERLINE);
	}
}
