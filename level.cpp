#include "level.h"

Level::Level() {
	std::ifstream file;
	file.open("map.txt");
	CharacterFactory factory;
	int height, width, knightX, knightY, princessX, princessY;
	file >> height >> width;
	file >> knightX >> knightY;
	file >> princessX >> princessY;
	map.resize(height, vector<shared_ptr<Character>>(width));
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

void Level::AddCharacter(shared_ptr<Character> c) {
	map[c->Position().x][c->Position().y] = c;
	characterList.push_back(c);
}