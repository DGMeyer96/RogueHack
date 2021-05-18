/*
*
* New renderer type.  This will replace the standard Sprite Renderer and include all added functionality from
* the Tilemap Renderer as well.  This new renderer will draw out in waves.  These waves include:
* 
* Static Game Objects (Do not move or change states at all)
* Item Game Objects (Will only change when the player interacts with them)
* Dynamic Game Objects (Will always change)
* Player Game Object (Changes with the player input, will update camera as well)
* UI Game Objects 
* 
* List of Game Objects will be held in a resizeable vector.  Every frame the Batch Renderer will receive a new
* vector that contains the list of Game Objects to be rendered.  For Static this will only change when the levels
* are changed, for Dynamic this will change every frame.  Each Game Object will contain information about its
* Transform (pos, rot, scale), a sprite, a color, and any other information specific to that Game Object.
* 
*/

#ifndef BATCH_RENDERER_H
#define BATCH_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Game_Math.h"
#include "Game_Object.h"

#include "texture.h"
#include "shader.h"

class BatchRenderer
{
public:
	BatchRenderer(Shader shader, Texture2D tilemap, glm::vec2 cellSize, unsigned int screen_width, unsigned int screen_height, float world_unit);
	~BatchRenderer();

    void SetRenderData(std::vector<GameObject> objectsToDraw, bool bUseScreenCoordinates);
    void SetTilemap(Texture2D tilemap, glm::vec2 cellSize);

    void UpdateTransforms(std::vector<GameObject> objectsToDraw);
    void UpdateTransforms_Unscaled(std::vector<GameObject> objectsToDraw);

    void BatchDraw(glm::vec2 position, float rotation = 0.0f, glm::vec2 size = glm::vec2(1.0f, 1.0f),   //Draw in World-Coordinates
        glm::vec3 color = glm::vec3(1.0f));
    void BatchDraw_Unscaled(glm::vec2 position, float rotation = 0.0f, glm::vec2 size = glm::vec2(1.0f, 1.0f),  // Draw in Pixel-Coordinates, used for UI
        glm::vec3 color = glm::vec3(1.0f));

    void UpdateCameraPosition(glm::vec2 cameraPos) { Camera_Position = cameraPos; }
    glm::vec2 GetCameraPosition() { return Camera_Position; }

    void UpdateWorldUnit(float worldUnit) { World_Unit = worldUnit; }

private:
    Shader       shader;
    Texture2D    Tilemap;
    unsigned int quadVAO, quadVBO;
    unsigned int Screen_Width, Screen_Height;
    float World_Unit;
    glm::vec2 World_Origin;
    glm::vec2 Camera_Position;
    glm::vec2 CellDimensions;

    int BATCH_SIZE = 512; // small = 128 (256^2 = 65,536), medium = 256 (512^2 = 262,144), large = 512 (1024^2 = 1,048,576)

    std::vector<glm::mat4> modelMatrices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec2> texOffsets;

	void InitRenderData();
    glm::mat4 UpdateModelMatrix(GameObject object);
    glm::mat4 UpdateModelMatrix_Unscaled(GameObject object);
};


#endif
