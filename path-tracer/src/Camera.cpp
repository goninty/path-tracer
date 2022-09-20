#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

glm::vec3 Camera::getPos() const { return pos; }

void Camera::moveForward()
{
	pos += look * speed;
}

void Camera::moveBackward()
{
	pos -= look * speed;
}

void Camera::moveLeft()
{
	pos -= right * speed;
}

void Camera::moveRight()
{
	pos += right * speed;
}
