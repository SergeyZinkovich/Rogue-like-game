#include "curses.h"
#include "main.h"

using std::vector;

Point::Point(int x0, int y0) {
	x = x0;               
	y = y0;
}

void GameManager::ChangeKnightDirection(Point dir) {
	knihhtPointer->SetDirection(dir);
}

void GameManager::AddCharacter(Character* &c) {
	map[c->Position().x][c->Position().y] = c;
	characterList.push_back(c);
}

GameManager::GameManager(){
	map.resize(5, vector<Character*>(5, new Character()));
	knihhtPointer = new Knight(Point(0, 0));
	map[0][0] = knihhtPointer;
	Dragon* d = new Dragon(Point(4, 4));
	map[4][4] = d;
	characterList.push_back(knihhtPointer);
	characterList.push_back(d);
}                   //вынести константы(мб добавить генерацию уровней)

void GameManager::Turn() {
	for (int i = 0; i < characterList.size(); i++) {
		characterList[i]->Move(map);
	}
}

const void GameManager::Draw() {
	clear();
	for (int i = 0; i < map.size(); i++) {
		for (int j = 0; j < map[0].size(); j++) {
			printw(map[i][j]->Symbol());
		}
		printw("\n");
	}
	refresh();
}

Character::Character(Point pos) {
	position = pos;
}

const int Character::HitPoints() {
	return hp;
}

const int Character::Damage() {
	return dmg;
}

const Point Character::Position() {
	return position;
}

const char* Character::Symbol() {
	return symbol;
}

Wall::Wall(Point pos) :
	Character(pos){
	symbol = "#";
	hp = INT16_MAX;           //вынести константы
	dmg = 0;
}

Movable::Movable(Point pos) :
	Character(pos) {}

void Movable::Move(vector<vector<Character*>> &map) {
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
	} while ((position.x + direction.x >= map.size()) || (position.x + direction.x < 0) ||
		(position.y + direction.y >= map[0].size()) || (position.y + direction.y < 0) ||
		(map[position.x + direction.x][position.y + direction.y]->Symbol() != "."));
	std::swap(map[position.x][position.y], map[position.x + direction.x][position.y + direction.y]);
	position.x += direction.x;
	position.y += direction.y;
}

Knight::Knight(Point pos) :
	Character(pos) {
	symbol = "K";
	hp = 300;                   //вынести константы
	dmg = 50;
	direction = Point(0, 0);
}

void Knight::Move(vector<vector<Character*>> &map) {
	if ((position.x + direction.x < map.size()) && (position.x + direction.x >= 0) && (position.y + direction.y < map[0].size()) && (position.y + direction.y >= 0)) {
		std::swap(map[position.x][position.y], map[position.x + direction.x][position.y + direction.y]);
		position.x += direction.x;
		position.y += direction.y;
	}
}

void Knight::SetDirection(Point dir) {
	direction = dir;
}

Princess::Princess(Point pos) :
	Character(pos) {
	symbol = "P";
	hp = 100;                  //вынести константы 
	dmg = 0;
}

void Princess::Move(vector<vector<Character*>> &map) {
	//подумать как она ходит 
}

Monster::Monster(Point pos) :
	Movable(pos){}

Dragon::Dragon(Point pos) :
	Monster(pos) {
	symbol = "D";
	hp = 1000;                 //вынести константы 
	dmg = 5;
}

Zombie::Zombie(Point pos) :
	Monster(pos) {
	symbol = "Z";
	hp = 100;
	dmg = 2;                      //вынести константы 
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

