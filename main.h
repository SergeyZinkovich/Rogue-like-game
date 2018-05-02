#pragma once
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

class Level;
class GameManager;
class Character;
class Movable;
class Knight;
class Wall;
class Floor;
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

class Level {
public:
	Level() {}
	Level(int height, int weight);
	vector<vector<shared_ptr<Character>>> map;
	vector<shared_ptr<Character>> characterList;
	shared_ptr<Knight> knightPointer;
private:
	void AddCharacter(shared_ptr<Character> c);
};

class GameManager {
public:
	GameManager();
	void ChangeKnightDirection(Point dir);
	void Turn();
	const void Draw();
private:
	Level _level;
};

class Character {
public:
	Character() {};
	Character(Point pos);
	const virtual int HitPoints();
	const virtual int Damage();
	const virtual Point Position();
	const virtual char* Symbol();
	const virtual bool IsDead();
	void TakeDamage(int damage);
	virtual void Collide(Character &other) {};
	virtual void Collide(Knight &other);
	virtual void Collide(Monster &other);
	virtual void Collide(Princess &other);
	virtual void Collide(Floor &other) {}
	virtual void Collide(Wall &other) {}
	virtual void Move(vector<vector<shared_ptr<Character>>> &map) {}
protected:
	bool _isDead;
	char* _symbol = ".";
	int _dmg;
	int _hp;
	Point _position;
};

class Wall : public Character {
public:
	Wall(Point pos);
};

class Floor : public Character {
public:
	Floor() {}
	void Collide(Character &other) override {};
	void Collide(Knight &other) override {}
	void Collide(Monster &other) override {}
	void Collide(Princess &other) override {}
	void Collide(Wall &other) override {}
};

class Potions : Character {

};

class HealingPotion : Potions {

};

class Projectiles : Character {
public:

private:
	Point _direction;
};

class Movable : public Character {
public:
	Movable(Point pos);
	void Move(vector<vector<shared_ptr<Character>>> &map) override;
};

class Knight : public Character {
public:
	Knight(Point pos);
	void Collide(Character &other) override;
	void Collide(Monster &other) override;
	void Collide(Princess &other) override;
	void Collide(Floor &other) override {}
	void Collide(Wall &other) override {}
	void Move(vector<vector<shared_ptr<Character>>> &map) override;
	void SetDirection(Point dir);
private:
	Point _direction;
};

class Princess : public Character {
public:
	Princess(Point pos);
	void Collide(Character &other) override;
	void Collide(Knight &other) override;
	void Collide(Monster &other) override;
	void Collide(Floor &other) override {}
	void Collide(Wall &other) override {}
	void Move(vector<vector<shared_ptr<Character>>> &map) override;
};

class Monster : public Movable {
public:
	Monster(Point pos);
	void Collide(Character &other) override;
	void Collide(Knight &other) override;
	void Collide(Princess &other) override;
	void Collide(Floor &other) override {}
	void Collide(Wall &other) override {}
};

class Dragon : public Monster {
public:
	Dragon(Point pos);
};

class Zombie : public Monster {
public:
	Zombie(Point pos);
};
