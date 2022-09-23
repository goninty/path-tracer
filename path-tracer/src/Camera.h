#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>

//#define look glm::vec3(0.0, 0.0, -1.0);
//#define up glm::vec3(0.0, 1.0, 0.0);

class Camera
{
// change these values (pos, look, up, s) to be taken into the constructor instead of initialized here
private:
	glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);

	// set default values for these vectors
	glm::vec3 look = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 right = glm::cross(look, up);

	// Default yaw value of -90 degrees looks along the -ve z axis.
	float yaw = -90.0f;
	float pitch = 0.0f;

	float movementSpeed = 0.1f;

	// ||g|| means length of g vector
	// create orthonormal uvw basis
	// use glm::normalize instead?
	/*
	glm::vec3 w = glm::normalize(-look);// -look / glm::length(look);
	glm::vec3 u = glm::normalize(glm::cross(up, w));// / glm::length(glm::cross(up, w));
	glm::vec3 v = glm::normalize(glm::cross(w, u));
	*/

	//glm::mat4 viewMatrix = glm::lookAt(pos, look, up);

	// distance to near viewing plane
	//glm::vec3 s = glm::vec3(0.0, 0.0, -1.0);

	// Note that, for rotations:
	// A horizontal rotation rotates around the world Y axis (always, irrespective of the camera's up vector)
	// A vertical rotation rotates around the camera's right vector (or, v vector, or whatever).

public:
	Camera();
	~Camera();
	// note that the middle value (center)
	// glm takes to be a point you're looking at
	// rather than a lookAt vector
	// hence it needs to be translated by pos
	glm::mat4 viewMatrix = glm::lookAt(pos, look + pos, up);
	glm::vec3 getPos() const;
	
	/*glm::vec3 look = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 right = glm::cross(look, up);*/

	glm::vec3 getLook() const;
	void setLook(glm::vec3 newLook);

	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();

	// delta ie "change in"
	// yaw and pitch are in degrees
	void rotate(float yawDelta, float pitchDelta);
};