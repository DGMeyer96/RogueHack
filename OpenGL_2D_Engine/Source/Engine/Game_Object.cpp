/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "Game_Object.h"


GameObject::GameObject()
    : Position(0.0f, 0.0f), Rotation(0.0f), Scale(1.0f, 1.0f), TextureCoordinates(0.0f, 0.0f), Velocity(0.0f), Color(1.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, float rotation, glm::vec2 scale, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Rotation(rotation), Scale(scale), Velocity(velocity), Color(color), Sprite(sprite), TextureCoordinates(0.0f, 0.0f), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, float rotation, glm::vec2 scale, glm::vec2 textureCoordinates, glm::vec3 color)
    : Position(pos), Rotation(rotation), Scale(scale), TextureCoordinates(textureCoordinates), Color(color), Velocity(0.0f), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Scale, this->Rotation, this->Color);
}