#include "Game_Math.h"

// calculates which direction a vector is facing (N,E,S or W)
Direction GameMath::VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

float GameMath::Magnitude(glm::vec2 vector2D)
{
    return sqrtf((vector2D.x * vector2D.x) + (vector2D.y * vector2D.y));
}

glm::vec2 GameMath::Normalize(glm::vec2 vector2D)
{
    return vector2D / Magnitude(vector2D);
}

float GameMath::DotProduct(glm::vec2 vector1, glm::vec2 vector2)
{
    return (vector1.x * vector2.x) + (vector1.y * vector2.y);
}

float GameMath::Lerp(float a, float b, float time)
{
    return (1 - time) * a + time * b;
}

glm::vec2 GameMath::Lerp(glm::vec2 vector1, glm::vec2 vector2, float time)
{
    return (1 - time) * vector1 + time * vector2;
}
