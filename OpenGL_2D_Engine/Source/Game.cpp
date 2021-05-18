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

//static const int MAP_SIZE = 1000;
//std::vector<std::vector<float>> PerlinNoiseMap(MAP_SIZE, std::vector<float>(MAP_SIZE));
std::vector<std::vector<float>> PerlinNoiseMap;

std::vector<GameObject> StaticObjects;
std::vector<GameObject> DynamicObjects;
std::vector<GameObject> ItemObjects;
std::vector<GameObject> PlayerObjects;
std::vector<GameObject> UIObjects;
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

    if (this->State == GAME_ACTIVE)
    {
        GameEngine->UpdatePlayerObjectPool(PlayerObjects);
        GameEngine->UpdateCameraPosition(Player_Object->Position, deltaTime * 5.0f);
    }
        
}

void Game::ProcessInput(float deltaTime)
{
    if (this->State == GAME_MENU)
    {
        MainMenu();

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

            //GameEngine->UpdateCamera(Player_Object->Position);
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

    // Toggle Performance Metrics
    if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
    {
        this->KeysProcessed[GLFW_KEY_P] = true;

        if (GameEngine->bDrawPerformanceMetrics)
            GameEngine->bDrawPerformanceMetrics = false;
        else
            GameEngine->bDrawPerformanceMetrics = true;
    }

    // Zoom Camera In
    if (this->Keys[GLFW_KEY_LEFT_BRACKET] && !this->KeysProcessed[GLFW_KEY_LEFT_BRACKET])
    {
        this->KeysProcessed[GLFW_KEY_LEFT_BRACKET] = true;

        if (CameraZoom == 0.1f)
            CameraZoom = 0.25f;
        else if (CameraZoom == 0.25f)
            CameraZoom = 1.0f;
        else if (CameraZoom == 1.0f)
            CameraZoom = 2.0f;

        std::cout << "ZOOM IN - " << CameraZoom << std::endl;
        GameEngine->UpdateCameraZoom(CameraZoom);
        GameEngine->UpdateStaticObjectPool(StaticObjects);
        
    }
    // Zoom Camera Out
    else if (this->Keys[GLFW_KEY_RIGHT_BRACKET] && !this->KeysProcessed[GLFW_KEY_RIGHT_BRACKET])
    {
        this->KeysProcessed[GLFW_KEY_RIGHT_BRACKET] = true;

        if (CameraZoom == 2.0f)
            CameraZoom = 1.0f;
        else if (CameraZoom == 1.0f)
            CameraZoom = 0.25f;
        else if (CameraZoom == 0.25f)
            CameraZoom = 0.1f;

        std::cout << "ZOOM OUT - " << CameraZoom << std::endl;
        GameEngine->UpdateCameraZoom(CameraZoom);
        GameEngine->UpdateStaticObjectPool(StaticObjects);
        
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
    UIObjects = std::vector<GameObject>();
    GameEngine->ClearTextObjects();
    
    //Draw Menu Specific Objects

    /*
    float scale = 1.0f;
    glm::vec3 color(1.0f, 1.0f, 1.0f);
    Text title("ROGUE HACK!", glm::vec2(Width * -0.08f, Height * 0.45f), scale, color);
    */

    glm::vec2 position(0.0f);
    float rotation = 0.0f;
    glm::vec2 scale(0.0f);

    //GameObject background = GameObject(position, rotation, glm::vec2(48.0f, 72.0f));
    //UIObjects.push_back(background);
    //GameEngine->UpdateUIObjectPool(UIObjects);
    //GameEngine->AddUIObject(background);


    Text text("ROGUE HACK!", glm::vec2(Width * -0.08f, Height * 0.45f));
    GameEngine->AddTextObject(text);

    text = Text("Press ENTER to start", glm::vec2(Width * -0.14f, 0.0f));
    GameEngine->AddTextObject(text);
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
    UIObjects = std::vector<GameObject>();
    GameEngine->ClearTextObjects();

    int smallOverworld = 32;
    int mediumOverworld = 64;
    int largeOverworld = 128;
    GenerateOverworld(largeOverworld);

    std::cout << "STATIC UPDATE" << std::endl;
    GameEngine->UpdateStaticObjectPool(StaticObjects);
    /*
    std::cout << "ITEM UPDATE" << std::endl;
    GameEngine->UpdateItemObjectPool(ItemObjects);
    std::cout << "DYNAMIC UPDATE" << std::endl;
    GameEngine->UpdateDynamicObjectPool(DynamicObjects);
    */
    Player_Object = new Player(ResourceManager::GetTexture("hero"));
    PlayerObjects = std::vector<GameObject>();
    PlayerObjects.push_back(*Player_Object);
    std::cout << "PLAYER UPDATE" << std::endl;
    GameEngine->UpdatePlayerObjectPool(PlayerObjects);

    GameUI();
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

void Game::GameUI()
{
    glm::vec2 position(0.0f);
    float rotation = 0.0f;
    glm::vec2 scale(1.0f);
    GameObject ui;

    /* LEFT UI ELEMENT */
    // Top Left
    position = glm::vec2(Width * -0.485f, Height * 0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(0.0f, 0.0f), Color::white());
    UIObjects.push_back(ui);
    // Top Right
    position = glm::vec2(Width * -0.3f, Height * 0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(2.0f, 0.0f), Color::white());
    UIObjects.push_back(ui);
    // Bottom Left
    position = glm::vec2(Width * -0.485f, Height * -0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(0.0f, 2.0f), Color::white());
    UIObjects.push_back(ui);
    // Bottom Right
    position = glm::vec2(Width * -0.3f, Height * -0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(2.0f, 2.0f), Color::white());
    UIObjects.push_back(ui);
    // Top Border
    position = glm::vec2(Width * -0.39f, Height * 0.475f);
    scale = glm::vec2(6.0f, 1.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 0.0f), Color::white());
    UIObjects.push_back(ui);
    // Bottom Border
    position = glm::vec2(Width * -0.39f, Height * -0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 2.0f), Color::white());
    UIObjects.push_back(ui);
    // Left Border
    position = glm::vec2(Width * -0.485f, 0.0f);
    scale = glm::vec2(1.0f, 18.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(0.0f, 1.0f), Color::white());
    UIObjects.push_back(ui);
    // Right Border
    position = glm::vec2(Width * -0.3f, 0.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(2.0f, 1.0f), Color::white());
    UIObjects.push_back(ui);
    // Filler
    position = glm::vec2(Width * -0.39f, 0.0f);
    scale = glm::vec2(6.0f, 18.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 1.0f), Color::white());
    UIObjects.push_back(ui);

    /* RIGHT UI ELEMENT */
    scale = glm::vec2(1.0f);
    // Top Left
    position = glm::vec2(Width * 0.3f, Height * 0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(0.0f, 0.0f), Color::white());
    UIObjects.push_back(ui);
    // Top Right
    position = glm::vec2(Width * 0.485f, Height * 0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(2.0f, 0.0f), Color::white());
    UIObjects.push_back(ui);
    // Bottom Left
    position = glm::vec2(Width * 0.3f, Height * -0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(0.0f, 2.0f), Color::white());
    UIObjects.push_back(ui);
    // Bottom Right
    position = glm::vec2(Width * 0.485f, Height * -0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(2.0f, 2.0f), Color::white());
    UIObjects.push_back(ui);
    // Top Border
    position = glm::vec2(Width * 0.39f, Height * 0.475f);
    scale = glm::vec2(6.0f, 1.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 0.0f), Color::white());
    UIObjects.push_back(ui);
    // Bottom Border
    position = glm::vec2(Width * 0.39f, Height * -0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 2.0f), Color::white());
    UIObjects.push_back(ui);
    // Left Border
    position = glm::vec2(Width * 0.3f, 0.0f);
    scale = glm::vec2(1.0f, 18.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(0.0f, 1.0f), Color::white());
    UIObjects.push_back(ui);
    // Right Border
    position = glm::vec2(Width * 0.485f, 0.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(2.0f, 1.0f), Color::white());
    UIObjects.push_back(ui);
    // Filler
    position = glm::vec2(Width * 0.39f, 0.0f);
    scale = glm::vec2(6.0f, 18.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 1.0f), Color::white());
    UIObjects.push_back(ui);

    /* BOTTOM UI ELEMENT */
    // Top Border
    position = glm::vec2(0.0f, Height * -0.3f);
    scale = glm::vec2(20.35f, 1.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 0.0f), Color::white());
    UIObjects.push_back(ui);
    // Bottom Border
    position = glm::vec2(0.0f, Height * -0.475f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 2.0f), Color::white());
    UIObjects.push_back(ui);
    // Filler
    position = glm::vec2(0.0f, Height * -0.375f);
    scale = glm::vec2(20.35f, 3.0f);
    ui = GameObject(position, rotation, scale, glm::vec2(1.0f, 1.0f), Color::white());
    UIObjects.push_back(ui);

    // Update UI object pool
    GameEngine->UpdateUIObjectPool(UIObjects);

    /* Add temp title message in message box */
    position = glm::vec2(Width * -0.12f, Height * -0.375f);
    Text titleText = Text("ROGUE HACK", position, 1.5f, Color::white());
    GameEngine->AddTextObject(titleText);
}

void Game::GenerateOverworld(int size)
{
    std::cout << "Generating Overworld" << std::endl;
    
    //PerlinNoiseMap.resize((size * 2) * (size * 2));

    GameObject temp;
    int index = 0;
    float offset = 0.0f;

    float min = 1.0f;
    float max = 0.0f;

    // Test implementation for STB perlin noise 
    //std::cout << "STB Noise Test" << std::endl;
    Timer t;
    t.StartTimer();
    float noiseVal;
    for (int x = -size; x < size; ++x)
    {
        for (int y = -size; y < size; ++y)
        {
            // Using '0' to specify "Don't care" for wrap 
            //noiseVal = stb_perlin_noise3(((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)), 0, 0, 0);

            int octaves = 6;        // number of "octaves" of noise3() to sum
            float lacunarity = 2.0f;// spacing between successive octaves (use exactly 2.0 for wrapping output)
            float gain = 0.5f;      // relative weighting applied to each successive octave (think of this as scale, 0.5f produces 0.1f -> 0.9f
            float offset = 1.0f;    // used to invert the ridges, may need to be larger, not sure

            
            // Ridge Fractal Perlin Noise (Range = 0 -> 1)
            noiseVal = stb_perlin_ridge_noise3(((float)rand() / (RAND_MAX)),
                                                ((float)rand() / (RAND_MAX)),
                                                ((float)rand() / (RAND_MAX)),
                                                lacunarity, gain, offset, octaves);
            
            std::cout << noiseVal << std::endl;

            if (noiseVal > max)
            {
                max = noiseVal;
            }
            else if (noiseVal < min)
            {
                min = noiseVal;
            }

            glm::vec2 translation;
            translation.x = (float)x + offset;
            translation.y = (float)y + offset;

            float rotation = 0.0f;
            glm::vec2 scale = glm::vec2(1.0f, 1.0f);

            glm::vec3 color(noiseVal, noiseVal, noiseVal);
            //glm::vec2 texCoords(((float)rand() / (RAND_MAX)) * 10.0f, ((float)rand() / (RAND_MAX)) * 10.0f);

            //temp = GameObject(translation, 0.0f, glm::vec2(1.0f, 1.0f), ResourceManager::GetTexture("block"), color, glm::vec2(0.0f, 0.0f));
            //temp = GameObject(translation, rotation, scale, texCoords, glm::vec3(1.0f, 1.0f, 1.0f));

            temp = GameObject(translation, rotation, scale, glm::vec2(15.0f, 0.0f), color);
            StaticObjects.push_back(temp);
        }
    }
    t.StopTimer();
    std::cout << "Elapsed Time: " << t.ElapsedTimeMS() << "ms" << std::endl;

    std::cout << "Min Value: " << min << "\t Max Value: " << max << std::endl;

    /*
    //int numObjects = 128;
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
    */
    std::cout << "Overworld Generation Done - " << StaticObjects.size() << " Objects Generated" << std::endl;
}

void Game::LoadOverworld()
{

}