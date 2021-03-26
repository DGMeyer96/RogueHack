/*******************************************************************
**
*******************************************************************/
// Game Includes
#include "Game.h"
#include "Player.h"
// Engine Includes
#include "Engine/Resource_Manager.h"
#include "Engine/Sprite_Renderer.h"
#include "Engine/Tilemap_Renderer.h"
#include "Engine/Batch_Renderer.h"
#include "Engine/Game_Object.h"
#include "Engine/Particle_Generator.h"
#include "Engine/Post_Processor.h"
#include "Engine/Text_Renderer.h"
#include "Engine/Timer.h"
// Audio Engine
#include <irrklang/irrKlang.h>
using namespace irrklang;


// C++ Includes
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
//#include <chrono>       /* C++11 time, more accurate for debugging, replaced by Timer.h */

float ShakeTime = 0.0f;

// Game-related State data
SpriteRenderer*     Renderer;
TilemapRenderer*    TRenderer;
BatchRenderer*      Renderer_Static;
BatchRenderer*      Renderer_Dynamic;
BatchRenderer*      Renderer_Items;
BatchRenderer*      Renderer_Player;
ParticleGenerator*  Particles;
PostProcessor*      Effects;
ISoundEngine*       SoundEngine = createIrrKlangDevice();
TextRenderer*       Text;

// Player
Player* Player_Object;

static const int MAP_SIZE = 1000;
std::vector<std::vector<float>> PerlinNoiseMap(MAP_SIZE, std::vector<float>(MAP_SIZE));

std::vector<GameObject> GameMap;
std::vector<GameObject> Dynamic;
std::vector<GameObject> Items;
std::vector<GameObject> PlayerObjects;

void DrawOnScreenMessage(std::string message) { Text->RenderText(message, -400.0f, -300.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)); }

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Level(0)
{
    WORLD_UNIT = Height * WORLD_SCALE;
}

Game::~Game()
{
    delete Renderer;
    delete Renderer_Static;
    delete Renderer_Dynamic;
    delete Renderer_Items;
    delete Renderer_Player;
    delete Player_Object;
    delete Particles;
    delete Effects;
    delete SoundEngine;
    delete Text;
}

void Game::Init()
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
    //ResourceManager::LoadTexture("awesomeface.png", true, "face");
    ResourceManager::LoadTexture("./Assets/Textures/block.png", false, "block");
    //ResourceManager::LoadTexture("block_solid.png", false, "block_solid");
    //ResourceManager::LoadTexture("particle.png", true, "particle");

    Player_Object = new Player(ResourceManager::GetTexture("hero"));
    TRenderer = new TilemapRenderer(ResourceManager::GetShader("tile"), ResourceManager::GetTexture("spriteSheet"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);

    Renderer_Static = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("spriteSheet"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Dynamic = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("enemy"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Items = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("chest"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    Renderer_Player = new BatchRenderer(ResourceManager::GetShader("batch"), ResourceManager::GetTexture("hero"), glm::vec2(16.0f, 16.0f), Width, Height, WORLD_UNIT);
    
    // Initalize random seed
    srand(time(NULL));

    std::chrono::steady_clock::time_point start, end;
    
    // Test implementation for STB perlin noise 
    std::cout << "STB Noise Test" << std::endl;
    Timer t;
    t.StartTimer();
    float noiseVal;
    for (int x = 0; x < MAP_SIZE; ++x)
    {
        for (int y = 0; y < MAP_SIZE; ++y)
        {

            // Using '0' to specify "Don't care" for wrap 
            noiseVal = stb_perlin_noise3(((float) rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)), 0, 0, 0);
            // Don't want to have a negative value
            if (noiseVal < 0.0f)
                noiseVal *= -1.0f;

            PerlinNoiseMap[x][y] = noiseVal;
            //std::cout << noiseVal << std::endl;
        }
    }
    t.StopTimer();
    std::cout << "Elapsed Time: " << t.ElapsedTimeMS() << "ms" << std::endl;
    
    /*
    // Test implementation for GLM perlin noise 
    std::cout << "GLM Noise Test" << std::endl;
    start = std::chrono::steady_clock::now();
    noiseVal;
    for (int x = 0; x < MAP_SIZE; ++x)
    {
        for (int y = 0; y < MAP_SIZE; ++y)
        {
            // Using '0' to specify "Don't care" for wrap 
            noiseVal = glm::perlin(glm::vec2((float)rand() / (RAND_MAX), (float)rand() / (RAND_MAX)));
            // Don't want to have a negative value
            if (noiseVal < 0.0f)
                noiseVal *= -1.0f;

            PerlinNoiseMap[x][y] = noiseVal;
            //std::cout << noiseVal << std::endl;
        }
    }
    end = std::chrono::steady_clock::now();
    std::cout << "Elapsed Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    */

    std::cout << "Generating GameMap" << std::endl;
    
    int numObjects = 128;
    GameObject temp;
    int index = 0;
    float offset = 0.0f;
    for (int y = -numObjects; y < numObjects; ++y)
    {
        for (int x = -numObjects; x < numObjects; ++x)
        {
            glm::vec2 translation;
            translation.x = (float)x + offset;
            translation.y = (float)y + offset;

            float rotation = 0.0f;
            glm::vec2 scale = glm::vec2(1.0f, 1.0f);

            glm::vec3 color(((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)));
            glm::vec2 texCoords(((float)rand() / (RAND_MAX)) * 10.0f, ((float)rand() / (RAND_MAX)) * 10.0f);

            //temp = GameObject(translation, 0.0f, glm::vec2(1.0f, 1.0f), ResourceManager::GetTexture("block"), color, glm::vec2(0.0f, 0.0f));
            temp = GameObject(translation, rotation, scale, texCoords, glm::vec3(1.0f, 1.0f, 1.0f));
            GameMap.push_back(temp);
        }
    }

    std::cout << "Map Generation Done" << std::endl;

    Renderer_Static->SetRenderData(GameMap);

    PlayerObjects.push_back(*Player_Object);
    Renderer_Player->SetRenderData(PlayerObjects);
}

void Game::Update(float deltaTime)
{
    //Renderer->UpdateCameraPosition(GameMath::Lerp(Renderer->GetCameraPosition(), glm::vec2(Player_Object->Position.x, -Player_Object->Position.y), 0.05f));

    // check for collisions
    this->DoCollisions();
    // update particles
    //Particles->Update(deltaTime, *Ball, 1, glm::vec2(Ball->Radius / 2.0f));
}

void Game::ProcessInput(float deltaTime)
{
    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]
            || this->Keys[GLFW_KEY_UP] && !this->KeysProcessed[GLFW_KEY_UP])
        {
            //this->Level = (this->Level + 1) % 4;
            this->KeysProcessed[GLFW_KEY_W] = true;
            this->KeysProcessed[GLFW_KEY_UP] = true;
        }
        else if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]
            || this->Keys[GLFW_KEY_DOWN] && !this->KeysProcessed[GLFW_KEY_DOWN])
        {
            /*
            if (this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
                */
            this->KeysProcessed[GLFW_KEY_S] = true;
            this->KeysProcessed[GLFW_KEY_DOWN] = true;
        }
    }
    if (this->State == GAME_ACTIVE)
    {
        if (!Player_Object->IsDead())
        {
            // Move Up
            if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]
                || this->Keys[GLFW_KEY_UP] && !this->KeysProcessed[GLFW_KEY_UP])
            {
                std::cout << "UP" << std::endl;
                Player_Object->Position += glm::vec2(0.0f, 1.0f);

                this->KeysProcessed[GLFW_KEY_W] = true;
                this->KeysProcessed[GLFW_KEY_UP] = true;
            }
            // Move Down
            else if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]
                || this->Keys[GLFW_KEY_DOWN] && !this->KeysProcessed[GLFW_KEY_DOWN])
            {
                std::cout << "DOWN" << std::endl;
                Player_Object->Position += glm::vec2(0.0f, -1.0f);

                this->KeysProcessed[GLFW_KEY_S] = true;
                this->KeysProcessed[GLFW_KEY_DOWN] = true;
            }
            // Move Left
            else if (this->Keys[GLFW_KEY_A] && !this->KeysProcessed[GLFW_KEY_A]
                || this->Keys[GLFW_KEY_LEFT] && !this->KeysProcessed[GLFW_KEY_LEFT])
            {
                std::cout << "LEFT" << std::endl;
                Player_Object->Position += glm::vec2(-1.0f, 0.0f);

                this->KeysProcessed[GLFW_KEY_A] = true;
                this->KeysProcessed[GLFW_KEY_LEFT] = true;
            }
            // Move Right
            else if (this->Keys[GLFW_KEY_D] && !this->KeysProcessed[GLFW_KEY_D]
                || this->Keys[GLFW_KEY_RIGHT] && !this->KeysProcessed[GLFW_KEY_RIGHT])
            {
                std::cout << "RIGHT" << std::endl;
                Player_Object->Position += glm::vec2(1.0f, 0.0f);

                this->KeysProcessed[GLFW_KEY_D] = true;
                this->KeysProcessed[GLFW_KEY_RIGHT] = true;
            }
        }
    }
    if (this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            //Effects->Chaos = false;
            this->State = GAME_MENU;
        }
    }

    if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
    {
        this->KeysProcessed[GLFW_KEY_P] = true;

        if (bDrawPerformanceMetrics)
            bDrawPerformanceMetrics = false;
        else
            bDrawPerformanceMetrics = true;
    }
}

void Game::Render()
{
    // draw background
    Renderer->DrawSprite(ResourceManager::GetTexture("background"),
        glm::vec2(0.0f, 0.0f), glm::vec2(48.0f, 27.0f), 0.0f);

    //DrawStatic();

    if (this->State == GAME_ACTIVE || this->State == GAME_MENU)
    {
        //Effects->BeginRender();
    }
    if (State == GAME_MENU)
    {
        Text->RenderText("Press ENTER to start", Width * -0.15f, Height * 0.25f, 1.0f);
        //Text->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);
    }
    else if (State == GAME_ACTIVE)
    {
        // Hard Camera Movement
        //Renderer->UpdateCameraPosition(glm::vec2(Player_Object->Position.x, -Player_Object->Position.y));
        //TRenderer->UpdateCameraPosition(glm::vec2(Player_Object->Position.x, -Player_Object->Position.y));

        // Smooth Lerp Camera
        TRenderer->UpdateCameraPosition(GameMath::Lerp(TRenderer->GetCameraPosition(), glm::vec2(Player_Object->Position.x, -Player_Object->Position.y), 0.05f));
        Renderer->UpdateCameraPosition(GameMath::Lerp(Renderer->GetCameraPosition(), glm::vec2(Player_Object->Position.x, -Player_Object->Position.y), 0.05f));
        Renderer_Static->UpdateCameraPosition(GameMath::Lerp(Renderer_Static->GetCameraPosition(), glm::vec2(Player_Object->Position.x, -Player_Object->Position.y), 0.05f));
        Renderer_Dynamic->UpdateCameraPosition(GameMath::Lerp(Renderer_Dynamic->GetCameraPosition(), glm::vec2(Player_Object->Position.x, -Player_Object->Position.y), 0.05f));
        Renderer_Items->UpdateCameraPosition(GameMath::Lerp(Renderer_Items->GetCameraPosition(), glm::vec2(Player_Object->Position.x, -Player_Object->Position.y), 0.05f));
        Renderer_Player->UpdateCameraPosition(GameMath::Lerp(Renderer_Player->GetCameraPosition(), glm::vec2(Player_Object->Position.x, -Player_Object->Position.y), 0.05f));

        DrawStatic();
        DrawItems();
        DrawDynamic();
        DrawPlayer();
        DrawUI();
    }
    else if (State == GAME_WIN)
    {
        Text->RenderText(
            "You WON!!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
        );
        Text->RenderText(
            "Press ENTER to retry or ESC to quit", 130.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
        );
    }
    
    //DrawOnScreenMessage("Debug Message Test");
    if(bDrawPerformanceMetrics)
        PerformanceMetrics();
}

void Game::ResetLevel()
{

}

void Game::ResetPlayer()
{

}

void Game::DrawStatic()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    position = glm::vec2(0.0f, 1.0f);
    Renderer->DrawSprite(ResourceManager::GetTexture("block"), position, scale, rotation, color);
    position = glm::vec2(0.0f, -1.0f);
    Renderer->DrawSprite(ResourceManager::GetTexture("block"), position, scale, rotation, color);

    /*
    // Draw our generated perlin noise map
    for (int x = 0; x < PerlinNoiseMap.size(); ++x)
    {
        for (int y = 0; y < PerlinNoiseMap[0].size(); ++y)
        {
            position = glm::vec2(x - (MAP_SIZE / 2), y - (MAP_SIZE / 2));
            //color = glm::vec3(PerlinNoiseMap[x][y]);

            if (PerlinNoiseMap[x][y] > 0.55f)
            {
                color = glm::vec3(0.7f, 0.7f, 0.7f);
            }
            else if (PerlinNoiseMap[x][y] > 0.2f)
            {
                color = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            else
            {
                color = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            Renderer->DrawSprite(ResourceManager::GetTexture("block"), position, scale, rotation, color);
        }
    }
    */

    position = glm::vec2(0.0f, 0.0f);
    //Renderer_Static->BatchDraw(ResourceManager::GetTexture("block"), position, scale, rotation, color);
    Renderer_Static->BatchDraw(position, scale, rotation, color);
}

void Game::DrawItems()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    position = glm::vec2(1.0f, 0.0f);
    Renderer->DrawSprite(ResourceManager::GetTexture("chest"), position, scale, rotation, color);
}

void Game::DrawDynamic()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    // Testing individual sprite
    position = glm::vec2(-1.0f, 0.0f);
    Renderer->DrawSprite(ResourceManager::GetTexture("enemy"), position, scale, rotation, color);
    
    // Testing Tilemap rendering
    position = glm::vec2(0.0f, 3.0f);
    TRenderer->DrawSprite(glm::vec2(5.0f, 5.0f), position, scale, rotation, color);
}

void Game::DrawPlayer()
{
    glm::vec2 scale(1.0f, 1.0f);
    glm::vec2 position(0.0f, 0.0f);
    float rotation = 0.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    // Draw the player last so they are on top
    //Player_Object->Draw(*Renderer);

    PlayerObjects = std::vector<GameObject>();
    PlayerObjects.push_back(*Player_Object);
    Renderer_Player->UpdateTransforms(PlayerObjects);
    Renderer_Player->BatchDraw(Player_Object->Position, Player_Object->Scale, Player_Object->Rotation, Player_Object->Color);
}

void Game::DrawUI()
{
    // draw UI
    Text->RenderText("ROGUE HACK!", Width * -0.08f, Height * 0.45f, 1.0f);
}

void Game::InitPerformanceMetrics()
{
    // Used for Performance Metrics
    lastTime = glfwGetTime();
    numFrames = 0;
    fps = 0.0f;
    frameTime = 0.0f;
}

void Game::PerformanceMetrics()
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

void Game::DoCollisions()
{
    
}

bool Game::CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
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

