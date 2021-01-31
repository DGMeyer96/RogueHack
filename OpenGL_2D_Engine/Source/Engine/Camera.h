#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Camera
{
public:
	Camera(glm::vec2 position);
	~Camera();
	void Draw();
	void Move(glm::vec2 position);
	glm::mat4 GetViewMatrix();

	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	float Zoom;
};

#endif // !CAMERA_H

