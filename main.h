#pragma once
#include <vector>

using std::vector;

class GameManager;
class Character;
class Movable;
class Knight;
class Princess;
class Monster;
class Dragon;
class Zombie;

class Point {
public:
	Point(){}
	Point(int x0, int y);
	int x, y;
};

class GameManager {
public:
	GameManager();
	void ChangeKnightDirection(Point dir);
	void Turn();
	const void Draw();
private:
	void AddCharacter(Character* &c);
	Knight* knihhtPointer;
	vector<vector<Character*>> map;
	vector<Character*> characterList;
};

class Character {
public:
	Character() {};
	Character(Point pos);
	const virtual int HitPoints();
	const virtual int Damage();
	const virtual Point Position();
	const virtual char* Symbol();
	virtual void Move(vector<vector<Character*>> &map) {}
protected:
	char* symbol = ".";
	int dmg;
	int hp;
	Point position;
};

class Wall : public Character {
public:
	Wall(Point pos);
};

class Movable : public Character {
public:
	Movable(Point pos);
	virtual void Move(vector<vector<Character*>> &map) override;
};

class Knight : public Character {
public:
	Knight(Point pos);
	virtual void Move(vector<vector<Character*>> &map) override;
	void SetDirection(Point dir);
private:
	Point direction;
};

class Princess : public Character {
public:
	Princess(Point pos);
	virtual void Move(vector<vector<Character*>> &map) override;
};

class Monster : public Movable {
public:
	Monster(Point pos);
};

class Dragon : public Monster {
public:
	Dragon(Point pos);
};

class Zombie : public Monster {
public:
	Zombie(Point pos);
};
