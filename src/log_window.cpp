#include "log_window.h"

#include "ncurses_funcs.h"

#define LOG_WIN_HEIGHT		(LINES - (LINES / 3) + 1)
#define LOG_WIN_WIDTH		COLS
#define LOG_WIN_START_Y	((LINES / 3) - 1)
#define LOG_WIN_START_X	0

log_window::log_window(){
	win = ncurses_funcs::create_newwin(LOG_WIN_HEIGHT, LOG_WIN_WIDTH, LOG_WIN_START_Y, LOG_WIN_START_X);
	angel_pointer = NULL;
	choice = 0;
}

void log_window::start(model_angel* ap){
	angel_pointer = ap;
}

log_window::~log_window(){
	delwin(win);
}

#include "debug.h"
#include "main_window_header.h"
#include "colors.h"
#include "scrolling_machine.h"
#include "net_common.h"

void log_window::draw(){
	resize();
	wclear(win);
	box(win, 0, 0);

	print_header(win, "Log");

	mvwprintw(win, 1, 1, "Type  Text");

	std::vector<message*> message_log = angel_pointer->get_log();

	if(message_log.size() == 0){
		wrefresh(win);
		return;
	}
	if(choice >= message_log.size()){
		choice = message_log.size() - 1;
	}

	int height;
	int width;
	getmaxyx(win, height, width);

	int shown_page;
	int num_view_pages;
	unsigned int max_items_on_shown_page;
	unsigned int starting_entry;
	int shown_window_choice;

	compute_range_of_page(LOG_WIN_HEIGHT - 3, message_log.size(), choice, &shown_page, &num_view_pages, &max_items_on_shown_page, &starting_entry, &shown_window_choice);

	int start_y = 2;

	if(shown_page > 0){
		mvwprintw(win, start_y, COLS/2, "^^^");
		start_y = 3;
	}

	//If more than one page and not on last page...
	if(num_view_pages > 1 && shown_page < num_view_pages - 1){
		mvwprintw(win, start_y + max_items_on_shown_page, COLS/2, "vvv");
	}

	mvwprintw(win, 1, 20, "%d / %d", shown_page, num_view_pages - 1);

	int y_offset = 0;
	for(unsigned int i = starting_entry; i < max_items_on_shown_page + starting_entry; i++){
		if(i >= message_log.size()){
			break;
		}

		mvwprintw(win, start_y + y_offset, 1, "%s   ", MSG_TYPES[message_log[i]->type]);
		if(message_log[i]->text != NULL){
			wprintw(win, "%s", message_log[i]->text);
		}

		if(shown_window_choice == y_offset){
			mvwchgat(win, start_y + y_offset, 1, COLS-2, A_REVERSE, 0, NULL);
		}

		y_offset++;
	}

	wrefresh(win);
}

void log_window::select_down(){
	std::vector<message*> messages = angel_pointer->get_log();
	if(messages.size() > 0){
		choice++;
		choice %= messages.size();
	}
}

void log_window::select_up(){
	std::vector<message*> messages = angel_pointer->get_log();
	if(messages.size() > 0){
		choice--;
		if(choice < 0){
			choice = messages.size() - 1;
		}
	}
}

void log_window::resize(){
	mvwin(win, LOG_WIN_START_Y, LOG_WIN_START_X);
	wresize(win, LOG_WIN_HEIGHT, LOG_WIN_WIDTH);
}
