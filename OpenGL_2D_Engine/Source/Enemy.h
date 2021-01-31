#ifndef ENEMY_H
#define ENEMY_H

#include "Engine/Game_Object.h"

class Enemy : GameObject
{
public:
	Enemy(unsigned int maxHealth, unsigned int damage, unsigned int armor,
		glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color);
	~Enemy();
	void Activate();
	void Deactivate();
	bool Attack();
	bool TakeDamage(unsigned int damage);
	bool Move();

private:
	glm::vec2 PlayerPosition;
	unsigned int Health;
	unsigned int Mana;
	unsigned int Damage;
	unsigned int Armor;
};

#endif

