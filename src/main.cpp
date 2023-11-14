#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

#include <ncurses.h>

#include "window_demon.h"

using std::string;

string execPipe(const char* cmd){
	char buffer[128];
	std::string result = "";
	FILE* pipe = popen(cmd, "r");
	if (!pipe) throw std::runtime_error("popen() failed!");
	try{
		while(fgets(buffer, sizeof(buffer), pipe) != NULL){
			result += buffer;
		}
	}
	catch(...){
		pclose(pipe);
		throw;
	}
	pclose(pipe);
	return result;
}

void init_curses(){
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	refresh();
}

void end_curses(){
	endwin();
}

void update_curses(window_demon& demon){
	//printw("Hello, World.\n");
}

//Calls ss and gives ips to demon.
void update_connections(window_demon& demon){
	string output = execPipe("ss -naptuH | grep \"127.0.0.1\" -v");

	std::stringstream ss(output);
	string line;
	std::vector<string*> lines;

	while(std::getline(ss, line)){
		if(!line.empty()){
			line.back() = '\0';
			lines.push_back(new string(line));
		}
	}

	demon.update_connections(lines);
}

void update_loop(window_demon& demon){
	while(true){
		update_curses(demon);
		update_connections(demon);
		if(getch() == 'q'){
			break;
		}
	}
}

int main(int argc, char* argv[]){

	init_curses();

	//nodelay(stdscr, true);
	timeout(500);
	window_demon demon = window_demon();
	update_loop(demon);
	
	end_curses();

	return 0;
}
