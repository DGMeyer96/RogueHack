#ifndef PLAYER_H
#define PLAYER_H

#include "Engine/Game_Object.h"

class Player : public GameObject
{
public:
	Player(Texture2D sprite);
	Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color);
	~Player();

	bool Attack();
	bool TakeDamage(unsigned int damage);
	bool IsDead();

private:
	bool Dead;
	unsigned int Health;
	const unsigned int MaxHealth = 100;
	unsigned int Mana;
	const unsigned int MaxMana = 100;
	unsigned int Gold;
	unsigned int Damage;
	unsigned int Armor;
};

#endif