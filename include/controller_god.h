#ifndef CONTROLLER_GOD_H
#define CONTROLLER_GOD_H

#include "window_demon.h"
#include "model_angel.h"

class controller_god{
	public:
		controller_god();
		int start();
		~controller_god();

		int update();
		int handle_input(int button_press);
	private:
		window_demon demon;
		model_angel angel;
};

#endif
