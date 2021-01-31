#ifndef ITEM_H
#define ITEM_H

#include "Engine/Game_Object.h"
#include "Player.h"

class Item : GameObject
{
public:
	Item();
	~Item();

	virtual void Use(Player* player);

	std::string Name;
	unsigned int Value;
	unsigned int Weight;
};

#endif