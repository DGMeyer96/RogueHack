/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#include "Sprite_Renderer.h"

SpriteRenderer::SpriteRenderer(Shader shader, unsigned int screen_width, unsigned int screen_height, float world_unit)
{
    this->shader = shader;
    this->Screen_Width = screen_width;
    this->Screen_Height = screen_height;
    this->World_Unit = world_unit;
    this->World_Origin = glm::vec2(Screen_Width / 2, Screen_Height / 2);
    this->InitRenderData();
    Camera_Position = glm::vec2(0.0f, 0.0f);
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::DrawSprite(Texture2D texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    position.y *= -1.0f;    // +Y = UP and -Y = DOWN
    // prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(World_Origin + ((position - Camera_Position) * World_Unit), 0.0f));   // Make (0,0) center of the screen

    // Note: Order is from Left to Right so Transform order of operations is in reverse order
    // This makes the sprite's origin at the center instead of the top-left
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));   // Move origin back to top left
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate object
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin from top left, to center for correct rotations
    
    model = glm::scale(model, glm::vec3(size * World_Unit, 1.0f));   // (1,1) scale is now a fixed world scale, regardless of resolution / aspect

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6); //Less efficient, requires 6 data sets
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //More efficient, requires only 4 data sets
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1); //Instanced draw call
    glBindVertexArray(0);
}

void SpriteRenderer::UpdateCameraPosition(glm::vec2 cameraPos)
{
    //cameraPos.y *= -1.0f;
    Camera_Position = cameraPos;
}

void SpriteRenderer::InitRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos          // tex
        // Origin @ Center
        0.5f, -0.5f,    1.0f, 0.0f,
        0.5f, 0.5f,     1.0f, 1.0f,
        -0.5f, -0.5f,   0.0f, 0.0f,
        -0.5f, 0.5f,    0.0f, 1.0f
        
        /* Origin @ Top-Left
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f
        */

        /* For GL_TRIANGLES 
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
        */
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}