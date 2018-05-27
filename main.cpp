#include "curses.h"
#include "main.h"

#include <fstream>

using std::vector;
using std::make_shared;

Point::Point(int x0, int y0) : x(x0), y(y0) {}

void GameManager::ChangeKnightDirection(Point dir) {
	_level.knightPointer->SetDirection(dir);
}

void Level::AddCharacter(shared_ptr<Character> c) {
	map[c->Position().x][c->Position().y] = c;
	characterList.push_back(c);
}

Level::Level() {
	std::ifstream file;
	file.open("map.txt");
	int height, width, knightX, knightY;
	file >> height >> width;
	file >> knightX >> knightY;
	map.resize(height, vector<shared_ptr<Character>>(width));
	CharacterFactory factory;
	char c;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			file >> c;
			if ((i == knightX - 1) && (j == knightY - 1)) {
				knightPointer = make_shared<Knight>(Point(i, j));
				map[i][j] = knightPointer;
			}
			else {
				map[i][j] = factory.create(c, Point(i, j));
			}
			if (map[i][j]->IsPlayable()) {
				characterList.push_back(map[i][j]);
			}
		}
	}
}

GameManager::GameManager(){
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

const void GameManager::Draw() {
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
			printw(_level.map[i][j]->Symbol());
		}
		printw("\n");
	}
	refresh();
}

void GameManager::SetKnightFire() {
	_level.knightPointer->SetFire();
}

Character::Character(Point pos) : _position(pos), _isDead(false) {}

int Character::HitPoints() const {
	return _hp;
}

int Character::ManaPoits() const {
	return _mp;
}

int Character::Damage() const {
	return _dmg;
}

Point Character::Position() const {
	return _position;
}

char* Character::Symbol() const {
	return _symbol;
}

bool Character::IsDead() const {
	return _isDead;
}

bool Character::IsPlayable() const {
	return _isPlayable;
}

void Character::TakeDamage(int damage) {
	_hp -= damage;
	if (_hp <= 0) {
		_isDead = true;
	}
}

Wall::Wall(Point pos) : Character(pos){
	_symbol = "#";
	_hp = INT16_MAX;  
	_mp = 0;
	_dmg = 0;
	_isPlayable = false;
}

Movable::Movable(Point pos) :
	Character(pos) {}

void Movable::Move(Level &level) {
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
	} while ((_position.x + direction.x >= level.map.size()) || (_position.x + direction.x < 0) ||
		(_position.y + direction.y >= level.map[0].size()) || (_position.y + direction.y < 0));
		this->Collide(*level.map[_position.x + direction.x][_position.y + direction.y]);
	if (level.map[_position.x + direction.x][_position.y + direction.y]->IsDead()) {
		level.map[_position.x + direction.x][_position.y + direction.y] = make_shared<Floor>();
	}
	if (level.map[_position.x + direction.x][_position.y + direction.y]->Symbol() == ".") {
		std::swap(level.map[_position.x][_position.y], level.map[_position.x + direction.x][_position.y + direction.y]);
		_position.x += direction.x;
		_position.y += direction.y;
	}
}

Knight::Knight(Point pos) :
	Character(pos) {
	_symbol = "K";
	_hp = 300;  
	_mp = 100;
	_dmg = 50;
	_isPlayable = true;
	_direction = Point(0, 0);
	_fireAtCurrentTurn = false;
}

void Knight::Move(Level &level) {
	if (_fireAtCurrentTurn) {
		_fireAtCurrentTurn = false;
		CreateFireball(level);
		return;
	}
	if ((_position.x + _direction.x < level.map.size()) && (_position.x + _direction.x >= 0) && 
		(_position.y + _direction.y < level.map[0].size()) && (_position.y + _direction.y >= 0)) {
			this->Collide(*level.map[_position.x + _direction.x][_position.y + _direction.y]);
		if (level.map[_position.x + _direction.x][_position.y + _direction.y]->IsDead()) {
			level.map[_position.x + _direction.x][_position.y + _direction.y] = make_shared<Floor>();
		}
		if (level.map[_position.x + _direction.x][_position.y + _direction.y]->Symbol() == ".") {
			std::swap(level.map[_position.x][_position.y], level.map[_position.x + _direction.x][_position.y + _direction.y]);
			_position.x += _direction.x;
			_position.y += _direction.y;
		}
	}
}

void Knight::SetDirection(Point dir) {
	_direction = dir;
}

void Knight::CreateFireball(Level &level) {
	int x = _position.x + _direction.x;
	int y = _position.y + _direction.y;
	if ((x < level.map.size()) && (x >= 0) && (y < level.map[0].size()) && (y >= 0)) {
		level.projectilesList.push_back(make_shared<Fireball>(Point(x, y), _direction));
		level.map[x][y]->Collide(*level.projectilesList.back());
		if (level.map[x][y]->IsDead()) {
			level.map[x][y] = make_shared<Floor>();
		}
		if (!level.projectilesList.back()->IsDead()) {
			level.map[x][y] = level.projectilesList.back();
		}
	}
}

void Knight::SetFire() {
	_fireAtCurrentTurn = true;
}

Princess::Princess(Point pos) :
	Character(pos) {
	_symbol = "P";
	_hp = 100;   
	_mp = 0;
	_dmg = 0;
	_isPlayable = true;
}

void Princess::Move(Level &level) {
	// 
}

Monster::Monster(Point pos) :
	Movable(pos){}

Dragon::Dragon(Point pos) :
	Monster(pos) {
	_symbol = "D";
	_hp = 1000;
	_mp = 0;
	_dmg = 5;
	_isPlayable = true;
}

Zombie::Zombie(Point pos) :
	Monster(pos) {
	_symbol = "Z";
	_hp = 100;
	_mp = 0;
	_dmg = 2;
	_isPlayable = true;
}

Potion::Potion(Point pos) :
	Character(pos) {}

HealingPotion::HealingPotion(Point pos) :
	Potion(pos) {
	_hp = 50;
	_symbol = "H";
}

Projectile::Projectile(Point pos, Point direction):
Character(pos){
	_direction = direction;
}

void Projectile::Move(Level &level) {
	if ((_position.x + _direction.x < level.map.size()) && (_position.x + _direction.x >= 0) &&
		(_position.y + _direction.y < level.map[0].size()) && (_position.y + _direction.y >= 0)) {
		if (level.map[_position.x + _direction.x][_position.y + _direction.y]->Symbol() == ".") {
			std::swap(level.map[_position.x][_position.y], level.map[_position.x + _direction.x][_position.y + _direction.y]);
			_position.x += _direction.x;
			_position.y += _direction.y;
		}
		else {
			this->Collide(*level.map[_position.x + _direction.x][_position.y + _direction.y]);
			if (level.map[_position.x + _direction.x][_position.y + _direction.y]->IsDead()) {
				level.map[_position.x + _direction.x][_position.y + _direction.y] = make_shared<Floor>();
			}
			if (_isDead) {
				level.map[_position.x][_position.y] = make_shared<Floor>();
			}
		}
	}
	else {
		level.map[_position.x][_position.y] = make_shared<Floor>();
		_isDead = true;
	}
}

Fireball::Fireball(Point pos, Point direction):
Projectile(pos, direction){
	_hp = 50;
	_symbol = "*";
	_isPlayable = true;
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

void Character::Collide(Potion &other) {
	return other.Collide(*this);
}

void Character::Collide(Projectile &other) {
	other.Collide(*this);
}

void Knight::Collide(Character &other) {
	other.Collide(*this);
}

void Knight::Collide(Monster &other) {
	TakeDamage(_dmg);
}

void Knight::Collide(Princess &other) {
	//Победа?
}

void Monster::Collide(Character &other) {
	other.Collide(*this);
}

void Monster::Collide(Knight &other) {
	TakeDamage(_dmg);
}

void Monster::Collide(Princess &other) {
	//Можно ли проиграть при смерти принцессы?
}

void Monster::Collide(Projectile &other) {
	other.Collide(*this);
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

void Potion::Collide(Character &other) {
	other.Collide(*this);
}

void Potion::Collide(Knight &other) {
	other.Collide(*this);
}

void HealingPotion::Collide(Character &other) {
	other.Collide(*this);
}

void HealingPotion::Collide(Knight &other) {
	other.TakeDamage(-_hp);
	_isDead = true;
}

void Projectile::Collide(Character &other) {
	other.Collide(*this);
}

void Projectile::Collide(Monster &other) {
	other.TakeDamage(_hp);
	_isDead = true;
}

CharacterFactory::CharacterFactory() {
	add_actor<Floor>();
	add_actor<Wall>();
	add_actor<Knight>();
	add_actor<Princess>();
	add_actor<HealingPotion>();
	add_actor<Zombie>();
	add_actor<Dragon>();
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
		if ((ch == 119) || (ch == 115) || (ch == 100) || (ch == 97) || (ch == 32)) {
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
			case 32:
				g.SetKnightFire();
				break;
			}
			g.Turn();
			g.Draw();
		}
	}
	endwin(); 

	return 0;
}

