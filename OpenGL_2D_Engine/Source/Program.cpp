/*******************************************************************
** 
*******************************************************************/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Images
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//Noise
#define STB_PERLIN_IMPLEMENTATION
//#include "stb_perlin.h"

//Fonts
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Game.h"
#include "Engine/Resource_Manager.h"

#include <iostream>

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 1280;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 720;

float DEFAULT_MONITOR_REFRESH = 62.0f;

Game RogueHack(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Configure the window
    glfwWindowHint(GLFW_RESIZABLE, true);
    glfwWindowHint(GLFW_DECORATED, true);
    glfwWindowHint(GLFW_FOCUSED, true);
    glfwWindowHint(GLFW_AUTO_ICONIFY, true);
    
    glfwWindowHint(GLFW_DOUBLEBUFFER, true);

    // Borderless Window
    /*
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    //glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);

    DEFAULT_MONITOR_REFRESH = mode->refreshRate;
    
    //GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Rogue Hack", NULL, NULL);
    */

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rogue Hack", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //  V-Sync, Not set = driver default, 1 = Enable, 0 = disable, keep on to prevent 1000+ fps
    glfwSwapInterval(1);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialize game
    // ---------------
    RogueHack.Init();

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // manage user input
        // -----------------
        RogueHack.ProcessInput(deltaTime);

        // update game state
        // -----------------
        RogueHack.Update(deltaTime);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        RogueHack.Render();

        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            RogueHack.Keys[key] = true;
        else if (action == GLFW_RELEASE)
        {
            RogueHack.Keys[key] = false;
            RogueHack.KeysProcessed[key] = false;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    glfwSetWindowAspectRatio(window, 16, 9);    //Forces 16:9 aspect ratio when resizing
}