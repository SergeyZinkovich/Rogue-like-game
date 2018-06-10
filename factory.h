#pragma once
#include <map>
#include <functional>
#include <memory>
#include "characters.h"

class Character;
class Knight;
class Wall;
class Floor;
class Princess;
class Dragon;
class Zombie;
class HealingPotion;

template <class BaseT>
class BaseFactory
{
public:
	template <class ActorT>
	void add_actor()
	{
		constructors_[ActorT(Point(0, 0)).Symbol()] = [](Point pos) {return new ActorT(pos); };
	}

	std::shared_ptr<BaseT> create(char symbol, Point pos)
	{
		return std::shared_ptr<BaseT>(constructors_[symbol](pos));
	}

private:
	std::map<char, std::function<BaseT*(Point)>> constructors_;
};

class CharacterFactory : public BaseFactory<Character>
{
public:
	explicit CharacterFactory() {
		add_actor<Floor>();
		add_actor<Wall>();
		add_actor<Knight>();
		add_actor<Princess>();
		add_actor<HealingPotion>();
		add_actor<Zombie>();
		add_actor<Dragon>();
	}
};