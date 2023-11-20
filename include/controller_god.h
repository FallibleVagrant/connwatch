#ifndef CONTROLLER_GOD_H
#define CONTROLLER_GOD_H

#include "window_demon.h"
#include "model_angel.h"

class controller_god{
	public:
		controller_god();
		~controller_god();

		int update();
		int handle_input(char button_press);
		window_demon* get_demon();
	private:
		window_demon demon;
		model_angel angel;
};

#endif
