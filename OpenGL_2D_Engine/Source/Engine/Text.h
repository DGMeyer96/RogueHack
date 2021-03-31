
#ifndef TEXT_H
#define TEXT_H

#include <glm/glm.hpp>
#include <string>

class Text 
{
public:
	std::string String;
	glm::vec2 Position;
	float Scale;
	glm::vec3 Color;

	Text(std::string str, glm::vec2 pos, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f))
		: String(str), Position(pos), Scale(scale), Color(color) { };
};

#endif
