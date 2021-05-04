/*******************************************************************
**
*******************************************************************/
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <tuple>

#include "Game_Level.h"
#include "Engine/Game_Math.h"
#include "Engine/Engine.h"

#define ENGINE_SHADER_PATH "./Shaders/Engine/"
#define SHADER_PATH "./Shaders/"
#define TEXTURE_PATH "./Assets/Textures/"
#define AUDIO_PATH "./Assets/Audio/"
#define MUSIC_PATH "./Assets/Audio/Music/"
#define SFX_PATH "./Assets/Audio/SFX"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// The scale of 1 unit in the game (this is a % of the resolution Y)
//const float WORLD_SCALE = 0.05f;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    bool                    Keys[1024];
    bool                    KeysProcessed[1024];
    unsigned int            Width, Height;
    std::vector<GameLevel>  Levels;
    unsigned int            Level;

    // constructor/destructor
    Game(unsigned int width, unsigned int height, Engine* game_engine);
    ~Game();

    // initialize game state (load all shaders/textures/levels)
    void Init();

    // game loop
    void ProcessInput(float deltaTime);
    void Update(float deltaTime);

    // Collisions
    void DoCollisions();

    void MainMenu();
    void PauseGame();
    void NewGame();
    void LoadGame();

    void LoadingScreen();
    void ChangeLevel();

    // Reset Game
    void ResetLevel();
    void ResetPlayer();

    void GameUI();

private:
    Engine* GameEngine;
    //float WORLD_UNIT;
};

#endif