#include "Camera.h"

#include <iostream>

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
	pos -= u * speed;
}

void Camera::moveRight()
{
	pos += u * speed;
	//viewMatrix = glm::lookAt(pos, look, up);
}
