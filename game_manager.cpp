#include <vector>
#include "curses.h"
#include "game_manager.h"
#include "level.h"
#include "point.h"

using std::vector;

void GameManager::ChangeKnightDirection(Point dir) {
	_level.knightPointer->SetDirection(dir);
}

GameManager::GameManager() {
	_level = Level();
}

void GameManager::Turn() {
	for (int i = 0; i < _level.projectilesList.size(); i++) {
		if (!_level.projectilesList[i]->IsDead()) {
			_level.projectilesList[i]->Move(_level);
		}
	}
	for (int i = 0; i < _level.characterList.size(); i++) {
		if (!_level.characterList[i]->IsDead()) {
			_level.characterList[i]->Move(_level);
		}
	}
}

void GameManager::Draw() const {
	if (_level.princessPointer->ReachedByKnight()) {
		DrawWin();
	}
	else {
		DrawField();
	}
}

void GameManager::DrawField() const {
	clear();
	char c[10];
	sprintf(c, "%d", _level.knightPointer->HitPoints());
	printw(c);
	printw("\n");
	sprintf(c, "%d", _level.knightPointer->ManaPoits());
	printw(c);
	printw("\n");
	for (int i = 0; i < _level.map.size(); i++) {
		for (int j = 0; j < _level.map[0].size(); j++) {
			char k[2];
			k[0] = _level.map[i][j]->Symbol();
			k[1] = '\0';
			printw(k);
		}
		printw("\n");
	}
	refresh();
}

void GameManager::DrawWin() const {
	clear();
	printw("You win");
	refresh();
}

void GameManager::SetKnightFire() {
	_level.knightPointer->SetFire();
}