#include "Enemy.h"

Enemy::Enemy(unsigned int maxHealth, unsigned int damage, unsigned int armor,
			glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color)
{
	// Enemy Properties
	Health = maxHealth;
	Damage = damage;
	Armor = armor;

	// Game Object Properties
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Color = color;
	this->IsSolid = true;
	this->Destroyed = false;
}

Enemy::~Enemy()
{
}

void Enemy::Activate()
{
}

void Enemy::Deactivate()
{
}

bool Enemy::Attack()
{
	return false;
}

bool Enemy::TakeDamage(unsigned int damage)
{
	Health -= damage;

	// Return true if dead
	return Health > 0 ? false : true;
}

bool Enemy::Move()
{
	return false;
}
