/*
Basic CLUT for quick referencing colors
*/


#ifndef COLOR_H
#define COLOR_H

class Color
{
public:

	inline static glm::uvec3 black()	{ return glm::vec3(0.0f, 0.0f, 0.0f); }
	inline static glm::uvec3 blue()	{ return glm::vec3(0.0f, 0.0f, 1.0f); }
	inline static glm::uvec3 green()	{ return glm::vec3(0.0f, 1.0f, 0.0f); }
	inline static glm::uvec3 cyan()	{ return glm::vec3(0.0f, 1.0f, 1.0f); }
	inline static glm::uvec3 red()		{ return glm::vec3(1.0f, 0.0f, 0.0f); }
	inline static glm::uvec3 magenta() { return glm::vec3(1.0f, 0.0f, 1.0f); }
	inline static glm::uvec3 yellow()	{ return glm::vec3(1.0f, 1.0f, 0.3f); }
	inline static  glm::uvec3 brown()	{ return glm::vec3(1.0f, 0.5f, 1.0f); }
	inline static glm::uvec3 grey()	{ return glm::vec3(0.5f, 0.5f, 0.5f); }
	inline static glm::uvec3 white()	{ return glm::vec3(1.0f, 1.0f, 1.0f); }

private:
	Color() {}
};

#endif
