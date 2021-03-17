#include "Batch_Renderer.h"

BatchRenderer::BatchRenderer(Shader shader, unsigned int screen_width, unsigned int screen_height, float world_unit)
{
    this->shader = shader;
    this->Screen_Width = screen_width;
    this->Screen_Height = screen_height;
    this->World_Unit = world_unit;
    this->World_Origin = glm::vec2(Screen_Width / 2, Screen_Height / 2);
    this->InitRenderData();
    Camera_Position = glm::vec2(0.0f, 0.0f);

    //translations = new glm::vec2[BATCH_SIZE];
}

BatchRenderer::~BatchRenderer()
{
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    translations = std::vector<glm::vec2>();
}

void BatchRenderer::BatchDraw(Texture2D texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
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
    
    // draw 100 instanced quads
    glBindVertexArray(this->quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, translations.size()); // 100 triangles of 6 vertices each
    glBindVertexArray(0);
}

void BatchRenderer::InitRenderData()
{
    int index = 0;
    float offset = 0.0f;
    for (int y = -BATCH_SIZE; y < BATCH_SIZE; ++y)
    {
        for (int x = -BATCH_SIZE; x < BATCH_SIZE; ++x)
        {
            glm::vec2 translation;
            translation.x = (float)x + offset;
            translation.y = (float)y + offset;
            //translations[index++] = translation;
            translations.push_back(translation);
        }
    }

    // store instance data in an array buffer
    // --------------------------------------
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 1000000, &translations[0], GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * translations.size(), &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos          // tex
        // Origin @ Center
        0.5f, -0.5f,    1.0f, 0.0f,
        0.5f, 0.5f,     1.0f, 1.0f,
        -0.5f, -0.5f,   0.0f, 0.0f,
        -0.5f, 0.5f,    0.0f, 1.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // also set instance data
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(1, 1); // tell OpenGL this is an instanced vertex attribute.
}
