#ifndef GAME_MATH_H
#define GAME_MATH_H

#include <vector>
#include <iostream>
#include <map>
#include <math.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

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
    float Magnitude(glm::vec2 vector2D);
    glm::vec2 Normalize(glm::vec2 vector2D);
    float DotProduct(glm::vec2 vector1, glm::vec2 vector2);
    float Lerp(float a, float b, float time);
    glm::vec2 Lerp(glm::vec2 vector1, glm::vec2 vector2, float time);

private:
	GameMath() { }
};

#endif

