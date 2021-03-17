

#ifndef OVERWORLD_H
#define OVERWORLD_H

// OpenGL Includes
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// C++ Includes
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
// STB Perlin Noise Generation
#include "stb_perlin.h"

class Overworld
{
public:
	Overworld();
	Overworld(unsigned int map_size);
	~Overworld();

	void Draw();

private:
	unsigned int Map_Size;
	std::vector<std::vector<float>> PerlinNoiseMap;

};
#endif