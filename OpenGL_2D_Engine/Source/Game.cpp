/*******************************************************************
**
*******************************************************************/
// Game Includes
#include "Game.h"
#include "Player.h"
// Engine Includes
#include "Engine/Resource_Manager.h"
#include "Engine/Sprite_Renderer.h"
#include "Engine/Game_Object.h"
#include "Engine/Particle_Generator.h"
#include "Engine/Post_Processor.h"
#include "Engine/Text_Renderer.h"


// Audio Engine
#include <irrklang/irrKlang.h>
using namespace irrklang;

float ShakeTime = 0.0f;

// Game-related State data
SpriteRenderer*     Renderer;
//GameObject*         Player;
ParticleGenerator*  Particles;
PostProcessor*      Effects;
ISoundEngine*       SoundEngine = createIrrKlangDevice();
TextRenderer*       Text;

// Player
Player* Player_Object;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Level(0)
{
    WORLD_UNIT = Height * WORLD_SCALE;
}

Game::~Game()
{
    delete Renderer;
    delete Player_Object;
    delete Particles;
    delete Effects;
    delete SoundEngine;
    delete Text;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("./Shaders/Engine/Sprite.vert", "./Shaders/Engine/Sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("./Shaders/Engine/Particle.vert", "./Shaders/Engine/Particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("./Shaders/Engine/Post_Processing.vert", "./Shaders/Engine/Post_Processing.frag", nullptr, "postprocessing");
    
    // Configure Camera
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    // configure shaders
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
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
    ResourceManager::LoadTexture("./Assets/Textures/SpriteSheet/1Bit_SpriteSheet.png", false, "spriteSheet");
    ResourceManager::LoadTexture("./Assets/Textures/floppydisk.png", true, "test");
    ResourceManager::LoadTexture("./Assets/Textures/hero.gif", true, "hero");
    ResourceManager::LoadTexture("./Assets/Textures/enemy.gif", true, "enemy");
    ResourceManager::LoadTexture("./Assets/Textures/chest.png", true, "chest");
    //ResourceManager::LoadTexture("awesomeface.png", true, "face");
    ResourceManager::LoadTexture("./Assets/Textures/block.png", false, "block");
    //ResourceManager::LoadTexture("block_solid.png", false, "block_solid");
    //ResourceManager::LoadTexture("particle.png", true, "particle");

    Player_Object = new Player(ResourceManager::GetTexture("hero"));
}

void Game::Update(float deltaTime)
{
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
                Player_Object->Position += glm::vec2(0.0f, -1.0f);

                this->KeysProcessed[GLFW_KEY_W] = true;
                this->KeysProcessed[GLFW_KEY_UP] = true;
            }
            // Move Down
            else if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]
                || this->Keys[GLFW_KEY_DOWN] && !this->KeysProcessed[GLFW_KEY_DOWN])
            {
                std::cout << "DOWN" << std::endl;
                Player_Object->Position += glm::vec2(0.0f, 1.0f);

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
}

void Game::Render()
{
    
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU)
    {
        //Effects->BeginRender();

        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
            glm::vec2(0.0f, 0.0f), glm::vec2(48.0f, 27.0f), 0.0f);

        Player_Object->Draw(*Renderer);
        
        // Testing drawing sprite sheet
        //Renderer->DrawSprite(ResourceManager::GetTexture("spriteSheet"),
        //    glm::vec2(0.0f, 0.0f));

        // Testing individual sprite
        glm::vec2 scale(1.0f, 1.0f);
        //glm::vec2 position((Width / 2), (Height / 2));
        glm::vec2 position(0.0f, 0.0f);
        float rotation = 0.0f;
        glm::vec3 color(1.0f, 1.0f, 1.0f);
        //Renderer->DrawSprite(ResourceManager::GetTexture("hero"), position, scale, rotation, color);    // Center
        position = glm::vec2(-1.0f, 0.0f);
        Renderer->DrawSprite(ResourceManager::GetTexture("enemy"), position, scale, rotation, color);
        position = glm::vec2(1.0f, 0.0f);
        Renderer->DrawSprite(ResourceManager::GetTexture("chest"), position, scale, rotation, color);
        position = glm::vec2(0.0f, 1.0f);
        Renderer->DrawSprite(ResourceManager::GetTexture("block"), position, scale, rotation, color);
        position = glm::vec2(0.0f, -1.0f);
        Renderer->DrawSprite(ResourceManager::GetTexture("block"), position, scale, rotation, color);
 

        // draw UI
        //std::stringstream ss; 
        //ss << this->Lives;
        Text->RenderText("ROGUE HACK!", Width * -0.08f, Height * 0.45f, 1.0f);
    }
    if (State == GAME_MENU)
    {
        Text->RenderText("Press ENTER to start", Width * -0.15f, Height * 0.25f, 1.0f);
        //Text->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);
    }
    else if (State == GAME_ACTIVE)
    {
        
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
    
}

void Game::ResetLevel()
{

}

void Game::ResetPlayer()
{

}

void Game::DoCollisions()
{
    
}

bool Game::CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

