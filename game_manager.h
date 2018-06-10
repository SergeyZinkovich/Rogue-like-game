#pragma once
#include "level.h"
#include "point.h"

class Level;

class GameManager {
public:
	GameManager();
	void ChangeKnightDirection(Point dir);
	void Turn();
	void Draw() const;
	void DrawField() const;
	void DrawWin() const;
	void DrawLoss() const;
	void SetKnightFire();
private:
	Level _level;
};