#include "Camera.h"

#include <iostream>

Camera::Camera()
{
}

Camera::~Camera()
{
}

glm::vec3 Camera::getPos() const { return pos; }

void Camera::setLook(glm::vec3 newLook)
{ 
	look = newLook;
	viewMatrix = glm::lookAt(pos, look + pos, up);
}

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
	//viewMatrix = glm::lookAt(pos, look, up);
}
