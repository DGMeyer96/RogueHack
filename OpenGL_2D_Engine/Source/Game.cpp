/*******************************************************************
**
*******************************************************************/
// Game Includes
#include "Game.h"
#include "Player.h"
// Engine Includes

// Audio Engine
#include <irrklang/irrKlang.h>
using namespace irrklang;


// C++ Includes
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
//#include <chrono>       /* C++11 time, more accurate for debugging, replaced by Timer.h */

float ShakeTime = 0.0f;

// Player
Player* Player_Object;

static const int MAP_SIZE = 1000;
std::vector<std::vector<float>> PerlinNoiseMap(MAP_SIZE, std::vector<float>(MAP_SIZE));

std::vector<GameObject> StaticObjects;
std::vector<GameObject> DynamicObjects;
std::vector<GameObject> ItemObjects;
std::vector<GameObject> PlayerObjects;
std::vector<Text> TextObjects;

Game::Game(unsigned int width, unsigned int height, Engine* game_engine) 
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Level(0)
{
    //WORLD_UNIT = Height * WORLD_SCALE;
    GameEngine = game_engine;
}

Game::~Game()
{
    delete Player_Object;

    StaticObjects = std::vector<GameObject>();
    DynamicObjects = std::vector<GameObject>();
    ItemObjects = std::vector<GameObject>();
    PlayerObjects = std::vector<GameObject>();
}

void Game::Init()
{   
    // Initalize random seed
    srand(time(NULL));
    
    /*
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
   */

    /*
    GameEngine->UpdateStaticObjectPool(StaticObjects);
    std::cout << "Updated Static Object Pool" << std::endl;

    Player_Object = new Player(ResourceManager::GetTexture("hero"));
    PlayerObjects.push_back(*Player_Object);
    GameEngine->UpdatePlayerObjectPool(PlayerObjects);
    std::cout << "Updated Player Object Pool" << std::endl;
    */

    /*
    Renderer_Static->SetRenderData(StaticObjects);

    PlayerObjects.push_back(*Player_Object);
    Renderer_Player->SetRenderData(PlayerObjects);
    */
}

void Game::Update(float deltaTime)
{
    //Renderer->UpdateCameraPosition(GameMath::Lerp(Renderer->GetCameraPosition(), glm::vec2(Player_Object->Position.x, -Player_Object->Position.y), 0.05f));

    // check for collisions
    this->DoCollisions();
    // update particles
    //Particles->Update(deltaTime, *Ball, 1, glm::vec2(Ball->Radius / 2.0f));

    if(this->State == GAME_ACTIVE)
        GameEngine->UpdatePlayerObjectPool(PlayerObjects);
}

void Game::ProcessInput(float deltaTime)
{
    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            NewGame();
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

            GameEngine->UpdateCamera(Player_Object->Position);
        }
    }
    if (this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            //Effects->Chaos = false;
            MainMenu();
        }
    }

    if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
    {
        this->KeysProcessed[GLFW_KEY_P] = true;

        if (GameEngine->bDrawPerformanceMetrics)
            GameEngine->bDrawPerformanceMetrics = false;
        else
            GameEngine->bDrawPerformanceMetrics = true;
    }
}

void Game::ResetLevel()
{

}

void Game::ResetPlayer()
{
    Player_Object = new Player(ResourceManager::GetTexture("hero"));
}

void Game::DoCollisions()
{
    
}

void Game::MainMenu()
{
    this->State = GAME_MENU;
    
    // Clear Objects to Draw
    StaticObjects = std::vector<GameObject>();
    DynamicObjects = std::vector<GameObject>();
    ItemObjects = std::vector<GameObject>();
    PlayerObjects = std::vector<GameObject>();
    
    //Draw Menu Specific Objects
}

void Game::PauseGame()
{
}

void Game::NewGame()
{
    this->State = GAME_ACTIVE;

    // Clear Objects to Draw
    StaticObjects = std::vector<GameObject>();
    DynamicObjects = std::vector<GameObject>();
    ItemObjects = std::vector<GameObject>();
    PlayerObjects = std::vector<GameObject>();

    std::cout << "Generating Static Objects" << std::endl;
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
            StaticObjects.push_back(temp);
        }
    }
    std::cout << "Static Object Generation Done" << std::endl;
    GameEngine->UpdateStaticObjectPool(StaticObjects);

    Player_Object = new Player(ResourceManager::GetTexture("hero"));
    PlayerObjects = std::vector<GameObject>();
    PlayerObjects.push_back(*Player_Object);
    GameEngine->UpdatePlayerObjectPool(PlayerObjects);
}

void Game::LoadGame()
{
}

void Game::LoadingScreen()
{
}

void Game::ChangeLevel()
{
}

