#pragma once
#include <memory>
#include <fstream>
#include <vector>
#include "point.h"
#include "characters.h"
#include "factory.h"

using std::vector;
using std::make_shared;
using std::shared_ptr;

class Character;
class Point;
class Princess;
class Knight;
class CharacterFactory;

class Level {
public:
	Level();
	vector<vector<shared_ptr<Character>>> map;
	vector<shared_ptr<Character>> characterList;
	shared_ptr<Knight> knightPointer;
	shared_ptr<Princess> princessPointer;
	vector<shared_ptr<Character>> projectilesList;
private:
	void AddCharacter(shared_ptr<Character> c);
};