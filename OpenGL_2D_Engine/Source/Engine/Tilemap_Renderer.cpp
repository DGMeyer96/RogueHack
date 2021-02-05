/*******************************************************************
**
******************************************************************/

#include "Tilemap_Renderer.h"

TilemapRenderer::TilemapRenderer(Shader shader, Texture2D texture, glm::vec2 cellSize, unsigned int screen_width, unsigned int screen_height, float world_unit)
{
    Tile_Shader = shader;
    Tilemap = texture;
    CellWidth = cellSize.x;
    CellHeight = cellSize.y;
    Screen_Width = screen_width;
    Screen_Height = screen_height;
    World_Unit = world_unit;
    World_Origin = glm::vec2(Screen_Width / 2, Screen_Height / 2);
    InitRenderData();
    Camera_Position = glm::vec2(0.0f, 0.0f);
}

TilemapRenderer::~TilemapRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void TilemapRenderer::DrawSprite(glm::vec2 cellPos, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    position.y *= -1.0f;    // +Y = UP and -Y = DOWN
    // prepare transformations
    this->Tile_Shader.Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(World_Origin + ((position - Camera_Position) * World_Unit), 0.0f));   // Make (0,0) center of the screen

    // Note: Order is from Left to Right so Transform order of operations is in reverse order
    // This makes the sprite's origin at the center instead of the top-left
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));   // Move origin back to top left
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate object
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin from top left, to center for correct rotations

    model = glm::scale(model, glm::vec3(size * World_Unit, 1.0f));   // (1,1) scale is now a fixed world scale, regardless of resolution / aspect

    // Make sure to scale down the coordinate position by the map and cell dimensions
    this->Tile_Shader.SetVector2f("texCoordOffset", glm::vec2(cellPos.x / (Tilemap.Width / CellWidth), cellPos.y / (Tilemap.Height / CellHeight)));
    this->Tile_Shader.SetMatrix4("model", model);
    this->Tile_Shader.SetVector3f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    Tilemap.Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1); //Instanced draw call
    glBindVertexArray(0);
}

void TilemapRenderer::InitRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos          // tex
        // Origin @ Center
        0.5f, -0.5f,    (CellWidth / Tilemap.Width), 0.0f,
        0.5f, 0.5f,     (CellWidth / Tilemap.Width), (CellHeight / Tilemap.Height),
        -0.5f, -0.5f,   0.0f, 0.0f,
        -0.5f, 0.5f,    0.0f, (CellHeight / Tilemap.Height)
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
