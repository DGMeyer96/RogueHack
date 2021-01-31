#include "Camera.h"

Camera::Camera(glm::vec2 position)
{
	Position = glm::vec3(position, 0.0f);
	Front = glm::vec3(0.0f, 0.0f, -1.0f);
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	Zoom = 1.0f;
}

Camera::~Camera()
{
}

void Camera::Draw()
{
}

void Camera::Move(glm::vec2 position)
{
	Position = glm::vec3(position, 0.0f);
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, WorldUp);
}
