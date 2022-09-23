#include "Camera.h"

#include <iostream>

Camera::Camera()
{
	std::cout << glm::to_string(right) << std::endl;
}

Camera::~Camera()
{
}

glm::vec3 Camera::getPos() const { return pos; }

glm::vec3 Camera::getLook() const { return look; }

void Camera::setLook(glm::vec3 newLook)
{ 
	look = newLook;
	viewMatrix = glm::lookAt(pos, look + pos, up);
}

void Camera::moveForward()
{
	pos += look * movementSpeed;
}

void Camera::moveBackward()
{
	pos -= look * movementSpeed;
}

void Camera::moveLeft()
{
	pos -= right * movementSpeed;
}

void Camera::moveRight()
{
	pos += right * movementSpeed;
	//viewMatrix = glm::lookAt(pos, look, up);
}

void Camera::rotate(float yawDelta, float pitchDelta)
{
	yaw += yawDelta;
	pitch += pitchDelta;

	if (pitch > 89.9f) pitch = 89.9f;
	if (pitch < -89.9f) pitch = -89.9f;

	// https://learnopengl.com/Getting-started/Camera
	look.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	look.y = sin(glm::radians(pitch));
	look.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	glm::normalize(look);

	// we also need to recalculate the right vector
	right = glm::cross(look, up);
	glm::normalize(right);

	viewMatrix = glm::lookAt(pos, look + pos, up);
}