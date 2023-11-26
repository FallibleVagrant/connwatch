#ifndef ALARUM_WINDOW_H
#define ALARUM_WINDOW_H

#include <ncurses.h>

#include "abstract_window.h"

class model_angel;

class alarum_window : public abstract_window{
	public:
		alarum_window();
		void start(model_angel* pointer_to_angel);
		~alarum_window();

		void draw();
		void resize();
		void start_animating(unsigned int init_ticker);
		bool is_animating();
	private:
		WINDOW* win;
		model_angel* angel_pointer;
		unsigned int initial_ticker;
		bool is_playing;
};

#endif
