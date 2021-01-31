/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"

class SpriteRenderer
{
public:
    SpriteRenderer(Shader shader, unsigned int screen_width, unsigned int screen_height, float world_unit);
    ~SpriteRenderer();

    void DrawSprite(Texture2D texture, glm::vec2 position,
        glm::vec2 size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f,
        glm::vec3 color = glm::vec3(1.0f));
private:
    Shader       shader;
    unsigned int quadVAO;
    unsigned int Screen_Width, Screen_Height;
    float World_Unit;
    glm::vec2 World_Origin;

    void InitRenderData();
};

#endif