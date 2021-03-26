
#ifndef ENGINE_H
#define ENGINE_H

// OpenGL Includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// C++ Includes
#include <vector>
#include <tuple>
// Engine Includes
#include "Game_Math.h"
#include "Resource_Manager.h"
#include "Sprite_Renderer.h"
#include "Tilemap_Renderer.h"
#include "Batch_Renderer.h"
#include "Game_Object.h"
#include "Particle_Generator.h"
#include "Post_Processor.h"
#include "Text_Renderer.h"
#include "Timer.h"
#include "Text.h"
// Audio Engine
#include <irrklang/irrKlang.h>
using namespace irrklang;

#define ENGINE_SHADER_PATH "./Shaders/Engine/"
#define SHADER_PATH "./Shaders/"
#define TEXTURE_PATH "./Assets/Textures/"
#define AUDIO_PATH "./Assets/Audio/"
#define MUSIC_PATH "./Assets/Audio/Music/"
#define SFX_PATH "./Assets/Audio/SFX"

// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>

// The scale of 1 unit in the game (this is a % of the resolution Y)
const float WORLD_SCALE = 0.05f;

class Engine
{
public:
    unsigned int            Width, Height;
    bool bDrawPerformanceMetrics;

    // Engine-related State data
    SpriteRenderer* Renderer;
    TilemapRenderer* TRenderer;
    BatchRenderer* Renderer_Static;
    BatchRenderer* Renderer_Dynamic;
    BatchRenderer* Renderer_Items;
    BatchRenderer* Renderer_Player;
    ParticleGenerator* Particles;
    PostProcessor* Effects;
    ISoundEngine* SoundEngine = createIrrKlangDevice();
    TextRenderer* Text;

    // constructor/destructor
    Engine(unsigned int width, unsigned int height);
    ~Engine();

    void DrawOnScreenMessage(std::string message) { Text->RenderText(message, -400.0f, -300.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)); }

    // initialize Engine (load all shaders/textures)
    void Init();

    // Render Loop
    void Render();

    // Collision Detection
    bool CheckCollision(GameObject& one, GameObject& two);

    // Update Camera Position data for each render layer
    void UpdateCamera(glm::vec2 player_pos);

    // Update Render Layer data
    void UpdateStaticObjectPool(std::vector<GameObject> objects) { Renderer_Static->SetRenderData(objects); }
    void UpdateItemObjectPool(std::vector<GameObject> objects) { Renderer_Items->SetRenderData(objects); }
    void UpdateDynamicObjectPool(std::vector<GameObject> objects) { Renderer_Dynamic->SetRenderData(objects); }
    void UpdatePlayerObjectPool(std::vector<GameObject> objects) { Renderer_Player->SetRenderData(objects); }

private:
    float WORLD_UNIT;

    const std::string Engine_Shader_Path = "./Shaders/Engine/";
    const std::string Shader_Path = "./Shaders/";
    const std::string Texture_Path = "./Assets/Textures/";

    // Render Layers
    void DrawStatic();
    void DrawItems();   
    void DrawDynamic();
    void DrawPlayer();
    void DrawUI();

    // Performance Metric
    double lastTime;
    int numFrames;
    float frameTime, fps;
    void InitPerformanceMetrics();
    void PerformanceMetrics();
};


#endif
