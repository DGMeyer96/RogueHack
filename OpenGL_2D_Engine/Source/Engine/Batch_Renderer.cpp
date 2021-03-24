#include "Batch_Renderer.h"
#include <chrono>

BatchRenderer::BatchRenderer(Shader shader, Texture2D tilemap, glm::vec2 cellSize, unsigned int screen_width, unsigned int screen_height, float world_unit)
{
    this->shader = shader;
    Tilemap = tilemap;
    this->Screen_Width = screen_width;
    this->Screen_Height = screen_height;
    this->World_Unit = world_unit;
    this->World_Origin = glm::vec2(Screen_Width / 2, Screen_Height / 2);
    //this->InitRenderData();
    Camera_Position = glm::vec2(0.0f, 0.0f);
    CellDimensions = cellSize;
}

BatchRenderer::~BatchRenderer()
{
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    // de-allocate memory
    colors = std::vector<glm::vec3>();
    texOffsets = std::vector<glm::vec2>();
    modelMatrices = std::vector<glm::mat4>();
}

void BatchRenderer::SetRenderData(std::vector<GameObject> objectsToDraw)
{
    //std::cout << "Setting Render Data" << std::endl;

    colors.resize(objectsToDraw.size());
    texOffsets.resize(objectsToDraw.size());
    modelMatrices.resize(objectsToDraw.size());

    for (int i = 0; i < objectsToDraw.size(); ++i)
    {
        colors[i] = objectsToDraw[i].Color;
        texOffsets[i] = glm::vec2(objectsToDraw[i].TextureCoordinates.x / (Tilemap.Width / CellDimensions.x), 
                                objectsToDraw[i].TextureCoordinates.y / (Tilemap.Height / CellDimensions.y));
        modelMatrices[i] = UpdateModelMatrix(objectsToDraw[i]);
    }

    //std::cout << "Initializing Render Data" << std::endl;

    InitRenderData();
}

void BatchRenderer::SetTilemap(Texture2D tilemap, glm::vec2 cellSize)
{
    Tilemap = tilemap;
    CellDimensions = cellSize;
}

void BatchRenderer::UpdateTransforms(std::vector<GameObject> objectsToDraw)
{
    std::chrono::steady_clock::time_point start, end;
    std::cout << "Batch Renderer Transform Update" << std::endl;
    start = std::chrono::steady_clock::now();

    modelMatrices.resize(objectsToDraw.size());
    for (int i = 0; i < objectsToDraw.size(); ++i)
    {
        modelMatrices[i] = UpdateModelMatrix(objectsToDraw[i]);
    }

    end = std::chrono::steady_clock::now();
    std::cout << "Elapsed Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}

void BatchRenderer::BatchDraw(glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    position.y *= -1.0f;    // +Y = UP and -Y = DOWN
    // prepare transformations
    this->shader.Use();

    glm::mat4 camera = glm::mat4(1.0f);
    camera = glm::translate(camera, glm::vec3(((position - Camera_Position) * World_Unit), 0.0f));   // Make (0,0) center of the screen

    // Note: Order is from Left to Right so Transform order of operations is in reverse order
    // This makes the sprite's origin at the center instead of the top-left
    camera = glm::translate(camera, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));   // Move origin back to top left
    camera = glm::rotate(camera, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate object
    camera = glm::translate(camera, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin from top left, to center for correct rotations

    camera = glm::scale(camera, glm::vec3(size, 1.0f));   // (1,1) scale is now a fixed world scale, regardless of resolution / aspect

    shader.SetMatrix4("camera", camera);

    //std::cout << "Batch Draw" << std::endl;

    glActiveTexture(GL_TEXTURE0);
    Tilemap.Bind();
    
    // draw instanced quads
    glBindVertexArray(this->quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, modelMatrices.size()); // 100 triangles of 6 vertices each
    glBindVertexArray(0);
}

void BatchRenderer::InitRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos          // tex
        // Origin @ Center
        0.5f, -0.5f,    (CellDimensions.x / Tilemap.Width), 0.0f,
        0.5f, 0.5f,     (CellDimensions.x / Tilemap.Width), (CellDimensions.y / Tilemap.Height),
        -0.5f, -0.5f,   0.0f, 0.0f,
        -0.5f, 0.5f,    0.0f, (CellDimensions.y / Tilemap.Height)
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // Configure instance buffers
    unsigned int instanceVBO_Model[4];

    // store instance data in an array buffer
    // --------------------------------------
    for (unsigned int i = 0; i < 4; i++) 
    {
        glGenBuffers(1, &instanceVBO_Model[i]);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_Model[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(modelMatrices[0]) * modelMatrices.size(), &modelMatrices[0], GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set instance data
    for (unsigned int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(1 + i);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_Model[i]);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
        glVertexAttribDivisor(1 + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int instanceVBO_Color;
    glGenBuffers(1, &instanceVBO_Color);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_Color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * colors.size(), &colors[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 1);

    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_Color);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 1);
    glVertexAttribDivisor(5, 1);    // Update Attribute 6 every 1 instance

    unsigned int instanceVBO_TexOffset;
    glGenBuffers(1, &instanceVBO_TexOffset);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_TexOffset);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texOffsets.size(), &texOffsets[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 1);

    glEnableVertexAttribArray(6);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_TexOffset);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 1);
    glVertexAttribDivisor(6, 1);    // Update Attribute 6 every 1 instance
}

glm::mat4 BatchRenderer::UpdateModelMatrix(GameObject object)
{
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(World_Origin + ((object.Position - Camera_Position) * World_Unit), 0.0f));   // Make (0,0) center of the screen
    // Note: Order is from Left to Right so Transform order of operations is in reverse order
    // This makes the sprite's origin at the center instead of the top-left
    model = glm::translate(model, glm::vec3(0.5f * object.Scale.x, 0.5f * object.Scale.y, 0.0f));   // Move origin back to top left
    model = glm::rotate(model, glm::radians(object.Rotation), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate object
    model = glm::translate(model, glm::vec3(-0.5f * object.Scale.x, -0.5f * object.Scale.y, 0.0f)); // Move origin from top left, to center for correct rotations
    model = glm::scale(model, glm::vec3(object.Scale * World_Unit, 1.0f));   // (1,1) scale is now a fixed world scale, regardless of resolution / aspect

    return model;
}
