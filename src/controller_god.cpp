#include "controller_god.h"

#include "model_angel.h"
#include "window_demon.h"

#include <stdio.h>

#include "common.h"

unsigned int ticker = 0;

controller_god::controller_god() : demon(), angel(demon){}

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

int controller_god::handle_input(char button_press){
	switch(button_press){
		case 'h':
			//angel.help();
			break;
		case ERR:
			//No button was pressed.
			break;
		default:
			return -1;
	}

	return 0;
}
