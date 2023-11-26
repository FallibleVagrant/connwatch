#include "warn_window.h"

#include "ncurses_funcs.h"
#include "common.h"

#include <cstring>

#include "model_angel.h"

#define WARN_WIN_HEIGHT		(LINES / 4)							//(LINES / 3)
#define WARN_WIN_WIDTH		((3 + (1/3)) * (LINES / 4))			//((COLS / 3) + 1) Also note that 3.3333 is roughly going to adjust WIDTH to square proportions.
#define WARN_WIN_START_Y	0
#define WARN_WIN_START_X	(COLS - WARN_WIN_WIDTH)

warn_window::warn_window(){
	win = ncurses_funcs::create_newwin(WARN_WIN_HEIGHT, WARN_WIN_WIDTH, WARN_WIN_START_Y, WARN_WIN_START_X);
}

void warn_window::start(model_angel* pointer_to_angel){
	this->angel_pointer = pointer_to_angel;
}

warn_window::~warn_window(){
	delwin(win);
}

const char* get_rand_char(int rand){
	const char* c;

	switch(rand){
		case 0:
			c = "#";
			break;
		case 1:
			c = "&";
			break;
		case 2:
			c = "+";
			break;
		case 3:
			c = "=";
			break;
		case 4:
			c = "*";
			break;
		case 5:
			c = "-";
			break;
		default:
			c = " ";
			break;
	}

	return c;
}

#include "colors.h"
#include "text_animations.h"

//These animations are cludgy!
//If you are here to change this system, rewrite it entirely.
void warn_window::draw(){
	resize();
	wclear(win);

	for(int i = 1; i < WARN_WIN_HEIGHT - 1; i++){
		wattron(win, COLOR_PAIR(WARN_AND_BLACK));
		//Print the streams of characters that appear on the left.
		if(1){
			const char* c = get_rand_char((ticker - i) % 11);
			if((ticker - i) % 143 <= 80){
				mvwprintw(win, i, 1, c);
			}
			c = get_rand_char((ticker - i) % 9);
			if((ticker - i) % 223 <= 100){
				mvwprintw(win, i, 2, c);
			}
			c = get_rand_char((ticker - i) % 7);
			if((ticker - i) % 349 <= 180){
				mvwprintw(win, i, 3, c);
			}
			c = get_rand_char((ticker - i) % 12);
			if((ticker - i) % 943 <= 200){
				mvwprintw(win, i, 4, c);
			}
		}
		//Print the streams of characters that appear on the right.
		if(1){
			const char* c = get_rand_char((ticker - i) % 11);
			if((ticker - i) % 139 <= 90){
				mvwprintw(win, i, WARN_WIN_WIDTH - 2, c);
			}
			c = get_rand_char((ticker - i) % 9);
			if((ticker - i) % 226 <= 130){
				mvwprintw(win, i, WARN_WIN_WIDTH - 3, c);
			}
			c = get_rand_char((ticker - i) % 7);
			if((ticker - i) % 363 <= 200){
				mvwprintw(win, i, WARN_WIN_WIDTH - 4, c);
			}
			c = get_rand_char((ticker - i) % 12);
			if((ticker - i) % 927 <= 200){
				mvwprintw(win, i, WARN_WIN_WIDTH - 5, c);
			}
		}

		//Print the bordering '|' characters on the left and right.
		wattroff(win, COLOR_PAIR(WARN_AND_BLACK));
		if((ticker + i) % 6 < 3){
			wattron(win, COLOR_PAIR(WARN_AND_BLACK));
			mvwprintw(win, i, 1, "|");
			mvwprintw(win, i, WARN_WIN_WIDTH - 2, "|");
			wattroff(win, COLOR_PAIR(WARN_AND_BLACK));
		}
		if(ticker % 13 + i % 5 <= 3){
			wattron(win, COLOR_PAIR(RED_AND_BLACK));
			mvwprintw(win, i, 1, ":");
			mvwprintw(win, i, WARN_WIN_WIDTH - 2, ":");
			wattroff(win, COLOR_PAIR(RED_AND_BLACK));
		}
	}

	int num_alerts = angel_pointer->get_num_alerts();
	int num_warnings = angel_pointer->get_num_warnings();

	const char** frame;
	unsigned int num_lines;
	if(num_alerts > 0){
		wattron(win, COLOR_PAIR(RED_AND_BLACK));
		const char* temp[] = {
			"   .   ",
			"  / \\  ",
			" / ! \\ ",
			"+-----+",
		};
		frame = temp;
		num_lines = 4;
	}
	else if(num_warnings > 0){
		wattron(win, COLOR_PAIR(WARN_AND_BLACK));
		const char* temp[] = {
			"   ",
			"   ",
			" O ",
			"/|\\",
			"   ",
		};
		frame = temp;
		num_lines = 5;
	}
	else{
		const char* temp[] = {
			"/ \\",
			"    ",
			"\\ /",
		};
		frame = temp;
		num_lines = 3;
	}
	const unsigned int num_cols = strlen(frame[0]);

	//Compute the horizontal and vertical shift applied to the frame every so often.
	for(unsigned int i = 0; i < num_lines; i++){
		int horizontal_glitch = 0;
		if((ticker + ((i << 3) % 5)) % 284 <= 37){
			horizontal_glitch = (((ticker + i) << 3) % 213) % 4;
			if(((ticker << 4) + ((i << 5) % 23)) % 2 == 0){
				horizontal_glitch *= -1;
			}
		}
		int vertical_glitch = 0;
		if((ticker + ((i << 2) % 7)) % 361 <= 25){
			vertical_glitch = (((ticker + i) << 3) % 213) % 3;
			if(((ticker << 5) + ((i << 4) % 49)) % 2 == 0){
				vertical_glitch *= -1;
			}
		}
		mvwprintw(win, i + (WARN_WIN_HEIGHT/2) - (num_lines/2) + vertical_glitch,
				horizontal_glitch + (WARN_WIN_WIDTH/2) - (num_cols/2), frame[i]);
	}

	//Reset colors from before.
	if(num_alerts > 0){
		wattroff(win, COLOR_PAIR(RED_AND_BLACK));
	}
	else if(num_warnings > 0){
		wattroff(win, COLOR_PAIR(WARN_AND_BLACK));
	}

	/*
	for(int i = 1; i < WARN_WIN_HEIGHT - 1; i++){
		int y = (((ticker - i) + 5) % 27);
		if((ticker) % 594 <= 225){
			y += 1;
		}
		if((ticker) % 487 <= 189){
			y += 1;
		}
		if((ticker) % 382 <= 231){
			y -= 1;
		}
		if((ticker) % 338 <= 248){
			y -= 1;
		}
		if(((ticker/2) - i) % 927 <= 200){
			if((ticker - i) % 9 <= 2){
				mvwprintw(win, i, (y % (WARN_WIN_WIDTH - 2)) + 1, "'");
			}
		}
	}
	*/

	//Box at the end so it is not occluded.
	box(win, 0, 0);

	if(num_alerts > 0){
		const char* text = "%d alerts";
		//Hack to find what the length will be after formatting.
		//https://stackoverflow.com/questions/26910479/find-the-length-of-a-formatted-string-in-c
		int len = snprintf(NULL, 0, text, num_alerts);
		mvwprintw(win, 0, (WARN_WIN_WIDTH/2) - len/2, text, num_alerts);
	}
	else{
		const char* text = "No alerts";
		unsigned int len = strlen(text);
		mvwprintw(win, 0, (WARN_WIN_WIDTH/2) - len/2, text, num_alerts);
	}
	if(num_warnings > 0){
		const char* text = "%d warnings";
		//Hack to find what the length will be after formatting.
		//https://stackoverflow.com/questions/26910479/find-the-length-of-a-formatted-string-in-c
		int len = snprintf(NULL, 0, text, num_warnings);
		mvwprintw(win, WARN_WIN_HEIGHT - 1, (WARN_WIN_WIDTH/2) - len/2, text, num_warnings);
	}
	else{
		const char* text = "No warnings";
		unsigned int len = strlen(text);
		mvwprintw(win, WARN_WIN_HEIGHT - 1, (WARN_WIN_WIDTH/2) - len/2, text, num_warnings);
	}
		//text = 
		//Maybe have a ref to angel?
		//also have color change upon warning.
		//Also a warning sign on alert.
		//and popup.

	wrefresh(win);
}

void warn_window::resize(){
	mvwin(win, WARN_WIN_START_Y, WARN_WIN_START_X);
	wresize(win, WARN_WIN_HEIGHT, WARN_WIN_WIDTH);
}
