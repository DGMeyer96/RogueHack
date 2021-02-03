#ifndef GAME_MATH_H
#define GAME_MATH_H

#include <vector>
#include <iostream>
#include <map>
#include <math.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
// LibNosie
#include <noise/noise.h>
#include <noise/noisegen.h>

// Directions for collision resolution
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

class GameMath
{
public:
	Direction VectorDirection(glm::vec2 closest);
    static float Magnitude(glm::vec2 vector2D);
    static glm::vec2 Normalize(glm::vec2 vector2D);
    static float DotProduct(glm::vec2 vector1, glm::vec2 vector2);
    static float Lerp(float a, float b, float time);
    static glm::vec2 Lerp(glm::vec2 vector1, glm::vec2 vector2, float time);

private:
	GameMath() { }
};

#endif

