
#include "Engine.h"

// C++ Includes
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
//#include <chrono>       /* C++11 time, more accurate for debugging, replaced by Timer.h */


Engine::Engine(unsigned int width, unsigned int height)
{
	Width = width;
	Height = height;
	WORLD_UNIT = Height * WORLD_SCALE;
}

Engine::~Engine()
{
    
    delete Renderer;
    delete Renderer_Static;
    delete Renderer_Dynamic;
    delete Renderer_Items;
    delete Renderer_Player;
    delete Particles;
    delete Effects;
    delete SoundEngine;
    delete Text;
    
}

void Engine::Init()
{
    InitPerformanceMetrics();

    // load shaders
    ResourceManager::LoadShader("./Shaders/Engine/Sprite.vert", "./Shaders/Engine/Sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("./Shaders/Engine/Tile.vert", "./Shaders/Engine/Tile.frag", nullptr, "tile");
    ResourceManager::LoadShader("./Shaders/Engine/Batch.vert", "./Shaders/Engine/Batch.frag", nullptr, "batch");
    ResourceManager::LoadShader("./Shaders/Engine/Particle.vert", "./Shaders/Engine/Particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("./Shaders/Engine/Post_Processing.vert", "./Shaders/Engine/Post_Processing.frag", nullptr, "postprocessing");

    // Configure Camera
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    // configure shaders
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("tile").Use().SetInteger("image", 0);
    ResourceManager::GetShader("tile").SetMatrix4("projection", projection);
    ResourceManager::GetShader("batch").Use().SetInteger("image", 0);
    ResourceManager::GetShader("batch").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"), Width, Height, WORLD_UNIT);
    //Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 1000);
    //Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), Width, Height);

    // Load BGM
    std::string BGM = AUDIO_PATH;
    BGM += "breakout.mp3";
    SoundEngine->play2D(BGM.c_str(), true);

    // Load Font
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("ocraext.TTF", 32);

    // load textures
    ResourceManager::LoadTexture("./Assets/Textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("./Assets/Textures/SpriteSheet/1Bit_SpriteSheet.png", true, "spriteSheet");
    ResourceManager::LoadTexture("./Assets/Textures/floppydisk.png", true, "test");
    ResourceManager::LoadTexture("./Assets/Textures/hero.gif", true, "hero");
    ResourceManager::LoadTexture("./Assets/Textures/enemy.gif", true, "enemy");
    ResourceManager::LoadTexture("./Assets/Textures/chest.png", true, "chest");
    ResourceManager::LoadTexture("./Assets/Textures/block.png", false, "block");

    TRenderer = new TilemapRenderer(ResourceManager::GetShader("tile"), ResourceManager::GetTexture("spriteSheet"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);

    Renderer_Static = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("spriteSheet"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Dynamic = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("enemy"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Items = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("chest"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Player = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("hero"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);

    // Initalize random seed
    srand(time(NULL));

}

void Engine::Render()
{
    DrawStatic();
    DrawItems();
    DrawDynamic();
    DrawPlayer();
    DrawUI();

    if (bDrawPerformanceMetrics)
        PerformanceMetrics();
}

bool Engine::CheckCollision(GameObject& one, GameObject& two)   // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Scale.x >= two.Position.x &&
        two.Position.x + two.Scale.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Scale.y >= two.Position.y &&
        two.Position.y + two.Scale.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

void Engine::UpdateCamera(glm::vec2 player_pos)
{
    // Smooth Lerp Camera
    TRenderer->UpdateCameraPosition(GameMath::Lerp(TRenderer->GetCameraPosition(), glm::vec2(player_pos.x, -player_pos.y), 0.05f));
    Renderer->UpdateCameraPosition(GameMath::Lerp(Renderer->GetCameraPosition(), glm::vec2(player_pos.x, -player_pos.y), 0.05f));
    Renderer_Static->UpdateCameraPosition(GameMath::Lerp(Renderer_Static->GetCameraPosition(), glm::vec2(player_pos.x, -player_pos.y), 0.05f));
    Renderer_Dynamic->UpdateCameraPosition(GameMath::Lerp(Renderer_Dynamic->GetCameraPosition(), glm::vec2(player_pos.x, -player_pos.y), 0.05f));
    Renderer_Items->UpdateCameraPosition(GameMath::Lerp(Renderer_Items->GetCameraPosition(), glm::vec2(player_pos.x, -player_pos.y), 0.05f));
    Renderer_Player->UpdateCameraPosition(GameMath::Lerp(Renderer_Player->GetCameraPosition(), glm::vec2(player_pos.x, -player_pos.y), 0.05f));
}

void Engine::DrawStatic()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    Renderer_Static->BatchDraw(position, scale, rotation, color);
}

void Engine::DrawItems()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    Renderer_Items->BatchDraw(position, scale, rotation, color);
}

void Engine::DrawDynamic()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    Renderer_Dynamic->BatchDraw(position, scale, rotation, color);
}

void Engine::DrawPlayer()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    Renderer_Player->BatchDraw(position, scale, rotation, color);
}

void Engine::DrawUI()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    Text->RenderText("ROGUE HACK!", Width * -0.08f, Height * 0.45f, 1.0f);
}

void Engine::InitPerformanceMetrics()
{
    // Used for Performance Metrics
    lastTime = glfwGetTime();
    numFrames = 0;
    fps = 0.0f;
    frameTime = 0.0f;
}

void Engine::PerformanceMetrics()
{
    // Measure speed
    double currentTime = glfwGetTime();
    numFrames++;
    if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
        // printf and reset timer
        frameTime = 1000.0 / (float)numFrames;
        fps = numFrames / (currentTime - lastTime);

        std::cout << "FPS: " << fps << std::endl;
        std::cout << "Frametime: " << frameTime << std::endl;

        //DrawOnScreenMessage("FPS: " + std::to_string(fps) + "\n" + "Frametime: " + std::to_string(frameTime));

        printf("%f ms/frame\n", 1000.0 / double(numFrames));
        numFrames = 0;
        lastTime += 1.0;
    }

    // draw UI
    Text->RenderText("FPS: " + std::to_string(fps), Width * -0.45f, Height * 0.45f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    Text->RenderText("Frametime: " + std::to_string(frameTime), Width * -0.45f, Height * 0.4f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}
