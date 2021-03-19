#include "Player.h"

Player::Player(Texture2D sprite)
{
	// Player Properties
	Dead = false;
	Health = MaxHealth;
	Mana = MaxMana;
	Gold = 0;
	Damage = 1;
	Armor = 0;

	// Game Object Properties
	this->Position = glm::vec2(0.0f, 0.0f);
	this->Scale = glm::vec2(1.0f, 1.0f);
	this->Sprite = sprite;
	this->Color = glm::vec3(1.0f, 1.0f, 1.0f);
	this->IsSolid = true;
	this->Destroyed = false;
}

Player::Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color)
{
	// Player Properties
	Dead = false;
	Health = MaxHealth;
	Mana = MaxMana;
	Gold = 0;
	Damage = 1;
	Armor = 0;

	// Game Object Properties
	this->Position = pos;
	this->Scale = size;
	this->Sprite = sprite;
	this->Color = color;
	this->IsSolid = true;
	this->Destroyed = false;
}

Player::~Player()
{
}

bool Player::Attack()
{
	return false;
}

bool Player::TakeDamage(unsigned int damage)
{
	Health -= damage;

	// Return true if dead
	return Health > 0 ? false : true;
}

bool Player::IsDead()
{
	return Dead;
}
