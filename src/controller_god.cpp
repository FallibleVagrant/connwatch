#include "controller_god.h"

#include "model_angel.h"
#include "window_demon.h"

#include <stdio.h>

#include "common.h"

unsigned int ticker = 761283810;

controller_god::controller_god() : demon(), angel(){}

#include "debug.h"

int controller_god::start(){
	int r = demon.start(&angel);
	if(r == -1){
		dbgprint("Demon could not start!\n");
		return -1;
	}

	r = angel.start(&demon);
	if(r == -1){
		dbgprint("Angel could not start!\n");
		return -1;
	}

	return 0;
}

controller_god::~controller_god(){}

int controller_god::update(){
	ticker += 1;

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
		case 'c':
			demon.show_connections();
			break;
		case 'o':
			demon.show_log();
			break;
		case 'n':
			demon.toggle_hostnames();
			break;
		case 'v':
			demon.toggle_services();
			break;
		case 'S':
			demon.toggle_sound();
			break;
		//Testing.
		case '0':
			angel.warn();
			break;
		case '9':
			angel.alert(NULL);
			break;
		case 'r':
			angel.reset_alerts_and_warns();
			break;
		case ERR:
			//No button was pressed.
			break;
		default:
			return -1;
	}

	return 0;
}
