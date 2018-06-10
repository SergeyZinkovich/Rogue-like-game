#pragma once
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <memory>
#include <string>
#include "configReader.h"
#include "point.h"
#include "level.h"

using std::vector;
using std::shared_ptr;

class Level;
class Character;
class Movable;
class Knight;
class Wall;
class Floor;
class Princess;
class Monster;
class Dragon;
class Zombie;
class Potion;
class HealingPotion;
class Projectile;
class Fireball;

extern ConfigReader config;

/*class Level {
public:
	Level();
	vector<vector<shared_ptr<Character>>> map;
	vector<shared_ptr<Character>> characterList;
	shared_ptr<Knight> knightPointer;
	shared_ptr<Princess> princessPointer;
	vector<shared_ptr<Character>> projectilesList;
private:
	void AddCharacter(shared_ptr<Character> c);
};*/

class Character {
public:
	Character() {};
	Character(Point pos);
	virtual int HitPoints() const;
	virtual int ManaPoits() const;
	virtual int Damage() const;
	virtual Point Position() const;
	virtual char Symbol() const;
	virtual bool IsDead() const;
	virtual bool IsPlayable() const;
	void TakeDamage(int damage);
	virtual void Collide(Character &other) {};
	virtual void Collide(Knight &other);
	virtual void Collide(Monster &other);
	virtual void Collide(Princess &other);
	virtual void Collide(Floor &other) {}
	virtual void Collide(Wall &other) {}
	virtual void Collide(Potion &other);
	virtual void Collide(Projectile &other);
	virtual void Move(Level &level) {}
protected:
	bool _isDead;
	bool _isPlayable;
	char _symbol = '.';
	int _dmg;
	int _hp;
	int _mp;
	Point _position;
};

class Wall : public Character {
public:
	Wall(Point pos);
	void Collide(Character &other) override {}
	void Collide(Knight &other) override {}
	void Collide(Monster &other) override {}
	void Collide(Projectile &other) override;
};

class Floor : public Character {
public:
	Floor() {}
	Floor(Point pos) {}
	void Collide(Character &other) override {};
	void Collide(Knight &other) override {}
	void Collide(Monster &other) override {}
	void Collide(Princess &other) override {}
	void Collide(Wall &other) override {}
	void Collide(Projectile &other) override {}
};

class Potion : public Character {
public:
	Potion(Point pos);
	void Collide(Character &other) override;
	void Collide(Knight &other) override;
	void Collide(Monster &other) override {}
	void Collide(Projectile &other) override;
};

class HealingPotion : public Potion {
public:
	HealingPotion(Point pos);
	void Collide(Character &other) override;
	void Collide(Knight &other) override;
};

class Projectile : public Character {
public:
	Projectile(Point pos, Point direction);
	void Move(Level &level);
	void Collide(Character &other) override;
	void Collide(Monster &other) override;
	void Collide(Wall &other) override { _isDead = true; }
	void Collide(Princess &other) override { _isDead = true; }
	void Collide(Potion &other) override { _isDead = true; }
	void Collide(Projectile &other) override { _isDead = true; }
protected:
	Point _direction;
};

class Fireball : public Projectile {
public:
	Fireball(Point pos, Point direction);
};

class Movable : public Character {
public:
	Movable(Point pos);
	void Move(Level &level) override;
};

class Knight : public Character {
public:
	Knight(Point pos);
	void Collide(Character &other) override;
	void Collide(Monster &other) override;
	void Collide(Princess &other) override;
	void Collide(Floor &other) override {}
	void Collide(Wall &other) override {}
	void Move(Level &level) override;
	void SetDirection(Point dir);
	void CreateFireball(Level &level);
	void SetFire();
private:
	Point _direction;
	bool _passTurn;
	bool _fireAtCurrentTurn;
};

class Princess : public Character {
public:
	Princess(Point pos);
	void Collide(Character &other) override;
	void Collide(Knight &other) override;
	void Collide(Monster &other) override;
	void Collide(Floor &other) override {}
	void Collide(Wall &other) override {}
	void Move(Level &level) override;
	void Collide(Projectile &other) override;
	void SetReached();
	bool ReachedByKnight() const;
private:
	bool _reachedByKnight;
};

class Monster : public Movable {
public:
	Monster(Point pos);
	void Collide(Character &other) override;
	void Collide(Knight &other) override;
	void Collide(Princess &other) override;
	void Collide(Floor &other) override {}
	void Collide(Wall &other) override {}
	void Collide(Projectile &other) override;
};

class Dragon : public Monster {
public:
	Dragon(Point pos);
};

class Zombie : public Monster {
public:
	Zombie(Point pos);
};