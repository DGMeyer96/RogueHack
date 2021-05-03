
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
    delete Renderer_Sprite;
    delete Renderer_Static;
    delete Renderer_Dynamic;
    delete Renderer_Items;
    delete Renderer_Player;
    delete Particles;
    delete Effects;
    delete SoundEngine;
    delete Renderer_Text;
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
    Renderer_Sprite = new SpriteRenderer(ResourceManager::GetShader("sprite"), Width, Height, WORLD_UNIT);
    //Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 1000);
    //Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), Width, Height);

    // Load BGM
    std::string BGM = AUDIO_PATH;
    BGM += "breakout.mp3";
    SoundEngine->play2D(BGM.c_str(), true);

    // Load Font
    Renderer_Text = new TextRenderer(this->Width, this->Height);
    Renderer_Text->Load("ocraext.TTF", 32);

    // load textures
    ResourceManager::LoadTexture("./Assets/Textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("./Assets/Textures/SpriteSheets/1Bit_SpriteSheet.png", true, "spriteSheet");
    ResourceManager::LoadTexture("./Assets/Textures/floppydisk.png", true, "test");
    ResourceManager::LoadTexture("./Assets/Textures/hero.gif", true, "hero");
    ResourceManager::LoadTexture("./Assets/Textures/enemy.gif", true, "enemy");
    ResourceManager::LoadTexture("./Assets/Textures/chest.png", true, "chest");
    ResourceManager::LoadTexture("./Assets/Textures/block.png", false, "block");

    ResourceManager::LoadTexture("./Assets/Textures/Palettes/Apple2.png", false, "apple2");
    ResourceManager::LoadTexture("./Assets/Textures/Palettes/CGA.png", false, "cga");
    ResourceManager::LoadTexture("./Assets/Textures/Palettes/Win16.png", false, "win16");

    ResourceManager::LoadTexture("./Assets/Textures/SpriteSheets/UI-Borders.png", true, "ui-borders");

    //TRenderer = new TilemapRenderer(ResourceManager::GetShader("tile"), ResourceManager::GetTexture("spriteSheet"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);

    Renderer_Static = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("spriteSheet"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Dynamic = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("enemy"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Items = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("chest"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Player = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("hero"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_UI = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("ui-borders"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);

    // Initalize random seed
    srand(time(NULL));
}

void Engine::Render()
{
    //std::cout << "Render Loop" << std::endl;

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

void Engine::UpdateCamera(glm::vec2 player_pos, float time)
{
    // Smooth Lerp Camera
    Renderer_Static->UpdateCameraPosition(GameMath::Lerp(Renderer_Static->GetCameraPosition(), glm::vec2(player_pos.x, -player_pos.y), time));
    Renderer_Items->UpdateCameraPosition(GameMath::Lerp(Renderer_Items->GetCameraPosition(), glm::vec2(player_pos.x, -player_pos.y), time));
    Renderer_Dynamic->UpdateCameraPosition(glm::vec2(player_pos.x, -player_pos.y));
}

void Engine::DrawStatic()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    //std::cout << "Static Object Draw STARTING" << std::endl;
    Renderer_Static->BatchDraw(position, rotation, scale, color);
   // std::cout << "Static Object Draw COMPLETE" << std::endl;
}

void Engine::DrawItems()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    //std::cout << "Items Object Draw STARTING" << std::endl;
    Renderer_Items->BatchDraw(position, rotation, scale, color);
    //std::cout << "Items Object Draw COMPLETE" << std::endl;
}

void Engine::DrawDynamic()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    //std::cout << "Dynamic Object Draw STARTING" << std::endl;
    Renderer_Dynamic->BatchDraw(position, rotation, scale, color);
    //std::cout << "Dynamic Object Draw COMPLETE" << std::endl;
}

void Engine::DrawPlayer()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    //std::cout << "Player Object Draw STARTING" << std::endl;
    Renderer_Player->BatchDraw(position, rotation, scale, color);
    //std::cout << "Player Object Draw COMPLETE" << std::endl;
}

void Engine::DrawUI()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    //Text->RenderText("ROGUE HACK!", Width * -0.08f, Height * 0.45f, 1.0f);

    //std::cout << "UI Object Draw STARTING" << std::endl;
    Renderer_UI->BatchDraw(position, rotation, scale, color);
    /*
    for (int i = 0; i < UIObjects.size(); ++i)
    {
        UIObjects[i].Draw(*Renderer_Sprite);
    }
    */
    for (int i = 0; i < UIText.size(); ++i)
    {
        Renderer_Text->RenderText(UIText[i]);
    }
    //std::cout << "UI Object Draw COMPLETE" << std::endl;
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
    Renderer_Text->RenderText("FPS: " + std::to_string(fps), Width * -0.45f, Height * 0.45f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    Renderer_Text->RenderText("Frametime: " + std::to_string(frameTime), Width * -0.45f, Height * 0.4f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}
