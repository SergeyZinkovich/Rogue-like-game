#include "curses.h"
#include <string>
#include <Windows.h>
#include <thread>
#include "game_manager.h"


int main() {  
	initscr();
	GameManager g = GameManager();
	g.Draw();
	noecho();
	raw();
	halfdelay(1);
	std::chrono::duration<long, std::ratio<5, 25>> frame_ratio{ 1 };
	while (true) {
		auto frame_start = std::chrono::steady_clock::now();
		int ch = getch();
		if ((ch == 119) || (ch == 115) || (ch == 100) || (ch == 97) || (ch == 32)) {
			if (ch == config.configJson["ButtonLeft"]) {
				g.ChangeKnightDirection(Point(-1, 0));
			}
			if (ch == config.configJson["ButtonRight"]) {
				g.ChangeKnightDirection(Point(1, 0));
			}
			if (ch == config.configJson["ButtonUp"]) {
				g.ChangeKnightDirection(Point(0, 1));
			}
			if (ch == config.configJson["ButtonDown"]) {
				g.ChangeKnightDirection(Point(0, -1));
			}
			if (ch == config.configJson["ButtonFire"]) {
				g.SetKnightFire();
			}
		}
		auto frame_end = std::chrono::steady_clock::now();
		auto delay = frame_ratio - (frame_end - frame_start);
		std::this_thread::sleep_for(delay);
		g.Turn();
		g.Draw();
	}
	endwin(); 

	return 0;
}

