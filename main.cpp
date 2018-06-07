#include "curses.h"
#include "main.h"
#include "configReader.cpp"
#include <fstream>
#include <string>
#include <Windows.h>
#include <thread>

using std::vector;
using std::make_shared;
using std::string;

ConfigReader config;

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
	int height, width, knightX, knightY, princessX, princessY;
	file >> height >> width;
	file >> knightX >> knightY;
	file >> princessX >> princessY;
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
			else if ((i == princessX - 1) && (j == princessY - 1)) {
				princessPointer = make_shared<Princess>(Point(i, j));
				map[i][j] = princessPointer;
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

char Character::Symbol() const {
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
	string s = config.configJson["Wall"]["symbol"];
	_symbol = s.c_str()[0];
	_hp = config.configJson["Wall"]["hp"];
	_mp = config.configJson["Wall"]["mp"];
	_dmg = config.configJson["Wall"]["dmg"];
	_isPlayable = config.configJson["Wall"]["isPlayable"];
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
	if (level.map[_position.x + direction.x][_position.y + direction.y]->Symbol() == '.') {
		std::swap(level.map[_position.x][_position.y], level.map[_position.x + direction.x][_position.y + direction.y]);
		_position.x += direction.x;
		_position.y += direction.y;
	}
}

Knight::Knight(Point pos) :
	Character(pos) {
	string s = config.configJson["Knight"]["symbol"];
	_symbol = s.c_str()[0];
	_hp = config.configJson["Knight"]["hp"];
	_mp = config.configJson["Knight"]["mp"];
	_dmg = config.configJson["Knight"]["dmg"];
	_isPlayable = config.configJson["Knight"]["isPlayable"];
	_direction = Point(0, 0);
	_passTurn = true;
	_fireAtCurrentTurn = false;
}

void Knight::Move(Level &level) {
	if (_fireAtCurrentTurn) {
		_fireAtCurrentTurn = false;
		CreateFireball(level);
		return;
	}
	if (_passTurn) { return; }
	if ((_position.x + _direction.x < level.map.size()) && (_position.x + _direction.x >= 0) && 
		(_position.y + _direction.y < level.map[0].size()) && (_position.y + _direction.y >= 0)) {
			this->Collide(*level.map[_position.x + _direction.x][_position.y + _direction.y]);
		if (level.map[_position.x + _direction.x][_position.y + _direction.y]->IsDead()) {
			level.map[_position.x + _direction.x][_position.y + _direction.y] = make_shared<Floor>();
		}
		if (level.map[_position.x + _direction.x][_position.y + _direction.y]->Symbol() == '.') {
			std::swap(level.map[_position.x][_position.y], level.map[_position.x + _direction.x][_position.y + _direction.y]);
			_position.x += _direction.x;
			_position.y += _direction.y;
		}
	}
	_passTurn = true;
}

void Knight::SetDirection(Point dir) {
	_direction = dir;
	_passTurn = false;
}

void Knight::CreateFireball(Level &level) {
	int x = _position.x + _direction.x;
	int y = _position.y + _direction.y;
	if ((x < level.map.size()) && (x >= 0) && (y < level.map[0].size()) && (y >= 0)) {
		level.projectilesList.push_back(make_shared<Fireball>(Point(x, y), _direction));
		level.projectilesList.back()->Collide(*level.map[x][y]);
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
	string s = config.configJson["Princess"]["symbol"];
	_symbol = s.c_str()[0];
	_hp = config.configJson["Princess"]["hp"];
	_mp = config.configJson["Princess"]["mp"];
	_dmg = config.configJson["Princess"]["dmg"];
	_isPlayable = config.configJson["Princess"]["isPlayable"];
	_reachedByKnight = false;
}

void Princess::SetReached() {
	_reachedByKnight = true;
}

bool Princess::ReachedByKnight() const{
	return _reachedByKnight;
}

void Princess::Move(Level &level) {
	// 
}

Monster::Monster(Point pos) :
	Movable(pos){}

Dragon::Dragon(Point pos) :
	Monster(pos) {
	string s = config.configJson["Dragon"]["symbol"];
	_symbol = s.c_str()[0];
	_hp = config.configJson["Dragon"]["hp"];
	_mp = config.configJson["Dragon"]["mp"];
	_dmg = config.configJson["Dragon"]["dmg"];
	_isPlayable = config.configJson["Dragon"]["isPlayable"];
}

Zombie::Zombie(Point pos) :
	Monster(pos) {
	string s = config.configJson["Zombie"]["symbol"];
	_symbol = s.c_str()[0];
	_hp = config.configJson["Zombie"]["hp"];
	_mp = config.configJson["Zombie"]["mp"];
	_dmg = config.configJson["Zombie"]["dmg"];
	_isPlayable = config.configJson["Zombie"]["isPlayable"];
}

Potion::Potion(Point pos) :
	Character(pos) {}

HealingPotion::HealingPotion(Point pos) :
	Potion(pos) {
	string s = config.configJson["HealingPotion"]["symbol"];
	_symbol = s.c_str()[0];
	_hp = config.configJson["HealingPotion"]["hp"];
	_mp = config.configJson["HealingPotion"]["mp"];
	_dmg = config.configJson["HealingPotion"]["dmg"];
	_isPlayable = config.configJson["HealingPotion"]["isPlayable"];
}

Projectile::Projectile(Point pos, Point direction):
Character(pos){
	_direction = direction;
}

void Projectile::Move(Level &level) {
	if ((_position.x + _direction.x < level.map.size()) && (_position.x + _direction.x >= 0) &&
		(_position.y + _direction.y < level.map[0].size()) && (_position.y + _direction.y >= 0)) {
		if (level.map[_position.x + _direction.x][_position.y + _direction.y]->Symbol() == '.') {
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
	string s = config.configJson["Fireball"]["symbol"];
	_symbol = s.c_str()[0];
	_hp = config.configJson["Fireball"]["hp"];
	_mp = config.configJson["Fireball"]["mp"];
	_dmg = config.configJson["Fireball"]["dmg"];
	_isPlayable = config.configJson["Fireball"]["isPlayable"];
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

void Wall::Collide(Projectile &other) {
	other.Collide(*this);
}

void Knight::Collide(Character &other) {
	other.Collide(*this);
}

void Knight::Collide(Monster &other) {
	TakeDamage(other.Damage());
}

void Knight::Collide(Princess &other) {
	other.SetReached();
}

void Monster::Collide(Character &other) {
	other.Collide(*this);
}

void Monster::Collide(Knight &other) {
	TakeDamage(other.Damage());
}

void Monster::Collide(Princess &other) {
	//ћожно ли проиграть при смерти принцессы?
}

void Monster::Collide(Projectile &other) {
	other.Collide(*this);
}

void Princess::Collide(Character &other) {
	return other.Collide(*this);
}

void Princess::Collide(Knight &other) {
	SetReached();
}

void Princess::Collide(Monster &other) {
	//≈сли она не двигаетс€, то у нее нет коллижинов?
}

void Princess::Collide(Projectile &other) {
	other.Collide(*this);
}

void Potion::Collide(Character &other) {
	other.Collide(*this);
}

void Potion::Collide(Knight &other) {
	other.Collide(*this);
}

void Potion::Collide(Projectile &other) {
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
	GameManager g = GameManager();
	g.Draw();
	noecho();
	raw();
	halfdelay(1);
	std::chrono::duration<long, std::ratio<3, 30>> frame_ratio{ 1 };
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

