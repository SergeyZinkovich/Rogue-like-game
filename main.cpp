#include "curses.h"
#include "main.h"

using std::vector;
using std::make_shared;

Point::Point(int x0, int y0) {
	x = x0;               
	y = y0;
}

void GameManager::ChangeKnightDirection(Point dir) {
	_level.knightPointer->SetDirection(dir);
}

void Level::AddCharacter(shared_ptr<Character> c) {
	map[c->Position().x][c->Position().y] = c;
	characterList.push_back(c);
}

Level::Level(int height, int weight) {
	map.resize(height, vector<shared_ptr<Character>>(weight));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < weight; j++) {
			map[i][j] = make_shared<Floor>();
		}
	}
	knightPointer = make_shared<Knight>(Point(0, 0));
	AddCharacter(knightPointer);
	AddCharacter(make_shared<Dragon>(Point(1, 0)));
}

GameManager::GameManager(){
	_level = Level(5, 5);
}                  

void GameManager::Turn() {
	for (int i = 0; i < _level.characterList.size(); i++) {
		if (!_level.characterList[i]->IsDead()) {
			_level.characterList[i]->Move(_level.map);
		}
	}
}

const void GameManager::Draw() {
	clear();
	for (int i = 0; i < _level.map.size(); i++) {
		for (int j = 0; j < _level.map[0].size(); j++) {
			printw(_level.map[i][j]->Symbol());
		}
		printw("\n");
	}
	refresh();
}

Character::Character(Point pos) {
	_position = pos;
	_isDead = false;
}

const int Character::HitPoints() {
	return _hp;
}

const int Character::Damage() {
	return _dmg;
}

const Point Character::Position() {
	return _position;
}

const char* Character::Symbol() {
	return _symbol;
}

const bool Character::IsDead() {
	return _isDead;
}

void Character::TakeDamage(int damage) {
	_hp -= damage;
	if (_hp <= 0) {
		_isDead = true;
	}
}

Wall::Wall(Point pos) :
	Character(pos){
	_symbol = "#";
	_hp = INT16_MAX;           
	_dmg = 0;
}

Movable::Movable(Point pos) :
	Character(pos) {}

void Movable::Move(vector<vector<shared_ptr<Character>>> &map) {
	Point direction;
	do {
		int way = rand() % 4;
		switch (way)
		{
		case 0:
			direction = Point(0, -1);
			break;
		case 1:
			direction = Point(0, 1);
			break;
		case 2:
			direction = Point(1, 0);
			break;
		case 3:
			direction = Point(-1, 0);
			break;
		}
	} while ((_position.x + direction.x >= map.size()) || (_position.x + direction.x < 0) ||
		(_position.y + direction.y >= map[0].size()) || (_position.y + direction.y < 0));
		this->Collide(*map[_position.x + direction.x][_position.y + direction.y]);
	if (map[_position.x + direction.x][_position.y + direction.y]->IsDead()) {
		map[_position.x + direction.x][_position.y + direction.y] = make_shared<Floor>();
	}
	if (map[_position.x + direction.x][_position.y + direction.y]->Symbol() == ".") {
		std::swap(map[_position.x][_position.y], map[_position.x + direction.x][_position.y + direction.y]);
		_position.x += direction.x;
		_position.y += direction.y;
	}
}

Knight::Knight(Point pos) :
	Character(pos) {
	_symbol = "K";
	_hp = 300;                   
	_dmg = 50;
	_direction = Point(0, 0);
}

void Knight::Move(vector<vector<shared_ptr<Character>>> &map) {
	if ((_position.x + _direction.x < map.size()) && (_position.x + _direction.x >= 0) && (_position.y + _direction.y < map[0].size()) && (_position.y + _direction.y >= 0)) {
			this->Collide(*map[_position.x + _direction.x][_position.y + _direction.y]);
		if (map[_position.x + _direction.x][_position.y + _direction.y]->IsDead()) {
			map[_position.x + _direction.x][_position.y + _direction.y] = make_shared<Floor>();
		}
		if (map[_position.x + _direction.x][_position.y + _direction.y]->Symbol() == ".") {
			std::swap(map[_position.x][_position.y], map[_position.x + _direction.x][_position.y + _direction.y]);
			_position.x += _direction.x;
			_position.y += _direction.y;
		}
	}
}

void Knight::SetDirection(Point dir) {
	_direction = dir;
}

Princess::Princess(Point pos) :
	Character(pos) {
	_symbol = "P";
	_hp = 100;                  
	_dmg = 0;
}

void Princess::Move(vector<vector<shared_ptr<Character>>> &map) {
	// 
}

Monster::Monster(Point pos) :
	Movable(pos){}

Dragon::Dragon(Point pos) :
	Monster(pos) {
	_symbol = "D";
	_hp = 1000;                  
	_dmg = 5;
}

Zombie::Zombie(Point pos) :
	Monster(pos) {
	_symbol = "Z";
	_hp = 100;
	_dmg = 2;                    
}

void Character::Collide(Knight &other) {
	return other.Collide(*this);
}

void Character::Collide(Monster &other) {
	return other.Collide(*this);
}

void Character::Collide(Princess &other) {
	return other.Collide(*this);
}

void Knight::Collide(Character &other) {
	return other.Collide(*this);
}

void Knight::Collide(Monster &other) {
	other.TakeDamage(this->_dmg);
}

void Knight::Collide(Princess &other) {
	//Победа?
}

void Monster::Collide(Character &other) {
	return other.Collide(*this);
}

void Monster::Collide(Knight &other) {
	other.TakeDamage(this->_dmg);
}

void Monster::Collide(Princess &other) {
	//Можно ли проиграть при смерти принцессы?
}

void Princess::Collide(Character &other) {
	return other.Collide(*this);
}

void Princess::Collide(Knight &other) {
	//Если она не двигается, то у нее нет коллижинов?
}

void Princess::Collide(Monster &other) {
	//Если она не двигается, то у нее нет коллижинов?
}

int main() {
	initscr();                  
	/*start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(1));*/
	GameManager g = GameManager();
	g.Draw();
	noecho();
	raw();
	while (true){
		int ch = getch();
		if ((ch == 119) || (ch == 115) || (ch == 100) || (ch == 97)) {
			switch (ch)
			{
			case 119:
				g.ChangeKnightDirection(Point(-1, 0));
				break;
			case 115:
				g.ChangeKnightDirection(Point(1, 0));
				break;
			case 100:
				g.ChangeKnightDirection(Point(0, 1));
				break;
			case 97:
				g.ChangeKnightDirection(Point(0, -1));
				break;
			}
			g.Turn();
			g.Draw();
		}
	}
	endwin(); 

	return 0;
}

