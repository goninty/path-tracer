#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

//#define look glm::vec3(0.0, 0.0, -1.0);
//#define up glm::vec3(0.0, 1.0, 0.0);

class Camera
{
// change these values (pos, look, up, s) to be taken into the constructor instead of initialized here
private:
	glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);

	// there is a fair amount of duplication here
	// (look, up, right and w, u, v are synomous save for a negative sign here or there)

	// set default values for these vectors
	glm::vec3 look = glm::vec3(0.0, 0.0, -1.0); 
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 right = glm::cross(look, up);

	// ||g|| means length of g vector
	// create orthonormal uvw basis
	// use glm::normalize instead?
	glm::vec3 w = -look / glm::length(look);
	glm::vec3 u = glm::cross(up, w) / glm::length(glm::cross(up, w));
	glm::vec3 v = glm::cross(w, u);

	//glm::mat4 viewMatrix = glm::lookAt(pos, look, up);

	// distance to near viewing plane
	glm::vec3 s = glm::vec3(0.0, 0.0, -1.0);

	float speed = 0.5f;

public:
	Camera();
	~Camera();
	glm::mat4 viewMatrix = glm::lookAt(pos, look, up);
	glm::vec3 getPos() const;
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
};