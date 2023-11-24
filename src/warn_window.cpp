#include "warn_window.h"

#include "ncurses_funcs.h"
#include "common.h"

#include <cstring>

//Save these settings for alert window?
/*#define WARN_WIN_HEIGHT		(LINES / 3)
#define WARN_WIN_WIDTH		((COLS / 3) + 1)
#define WARN_WIN_START_Y	((LINES / 3) - 1)
#define WARN_WIN_START_X	((COLS / 3) - 1)*/

#define WARN_WIN_HEIGHT		(LINES / 4)							//(LINES / 3)
#define WARN_WIN_WIDTH		((3 + (1/3)) * (LINES / 4))			//((COLS / 3) + 1) Also note that 3.3333 is roughly going to adjust WIDTH to square proportions.
#define WARN_WIN_START_Y	0
#define WARN_WIN_START_X	(COLS - WARN_WIN_WIDTH)

warn_window::warn_window(){
	win = ncurses_funcs::create_newwin(WARN_WIN_HEIGHT, WARN_WIN_WIDTH, WARN_WIN_START_Y, WARN_WIN_START_X);
	warning_level = 0;
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
	box(win, 0, 0);

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

	//Select the animation shown,
	//then the frame to show.
	int r = (((ticker / 23) + 2) << 3) % num_animations;
	const char*** animation = warn_animations[r];

	int frame_r = ticker % warn_animation_num_frames[r];

	const char** frame = animation[frame_r];
	const unsigned int num_lines = warn_animation_lens[r][frame_r];
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

	//mvwprintw(win, 1, (WARN_WIN_WIDTH/2) - length_to_first_newline, frame);
	const char* text;
	unsigned int text_len;
	if(warning_level == 0){
		text = "No alerts";
		text_len = strlen(text);
	}
	if(warning_level == 1){
		text = "NUM warnings";
	}
	if(warning_level == 2){
		//text = 
		//Maybe have a ref to angel?
		//also have color change upon warning.
	}
	mvwprintw(win, WARN_WIN_HEIGHT - 1, (WARN_WIN_WIDTH/2) - text_len/2, text);

	wrefresh(win);
}

void warn_window::resize(){
	mvwin(win, WARN_WIN_START_Y, WARN_WIN_START_X);
	wresize(win, WARN_WIN_HEIGHT, WARN_WIN_WIDTH);
}
