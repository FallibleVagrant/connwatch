#include "controller_god.h"

#include "model_angel.h"
#include "window_demon.h"

#include <stdio.h>

#include "common.h"

unsigned int ticker = 0;

controller_god::controller_god() : demon(), angel(&demon){}

controller_god::~controller_god(){}

int controller_god::update(){
	ticker += 1;
	ticker %= 3;

	int r = angel.update();
	if(r == -1){
		fprintf(stderr, "model_angel reported back error.\n");
		return -1;
	}

	r = demon.update();
	if(r == -1){
		fprintf(stderr, "window_demon reported back error.\n");
		return -1;
	}

	return 0;
}

//For ERR.
#include <ncurses.h>

int controller_god::handle_input(int button_press){
	switch(button_press){
		case KEY_RIGHT:
		case 'l':
			demon.select_right();
			break;
		case KEY_LEFT:
		case 'h':
			demon.select_left();
			break;
		case KEY_DOWN:
		case 'j':
			demon.select_down();
			break;
		case KEY_UP:
		case 'k':
			demon.select_up();
			break;
		case '0':
			demon.enter_bogus_mode();
			break;
		case 'a':
			demon.add_bogus_entry();
			break;
		case 'r':
			demon.rem_bogus_entry();
			break;
		case ERR:
			//No button was pressed.
			break;
		default:
			return -1;
	}

	return 0;
}
