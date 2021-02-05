/*******************************************************************
** 
******************************************************************/

#ifndef TILEMAP_RENDERER_H
#define TILEMAP_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Game_Math.h"

#include "texture.h"
#include "shader.h"

class TilemapRenderer
{
public:
    TilemapRenderer(Shader shader, Texture2D texture, glm::vec2 cellSize, unsigned int screen_width, unsigned int screen_height, float world_unit);
    ~TilemapRenderer();

    void DrawSprite(glm::vec2 cellPos, glm::vec2 position,
        glm::vec2 size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f,
        glm::vec3 color = glm::vec3(1.0f));

    void UpdateCameraPosition(glm::vec2 cameraPos) { Camera_Position = cameraPos; }
    glm::vec2 GetCameraPosition() { return Camera_Position; }

private:
    Shader      Tile_Shader;
    Texture2D   Tilemap;

    float CellWidth, CellHeight;

    unsigned int quadVAO;
    unsigned int Screen_Width, Screen_Height;

    float World_Unit;
    glm::vec2 World_Origin;
    glm::vec2 Camera_Position;

    void InitRenderData();
};

#endif