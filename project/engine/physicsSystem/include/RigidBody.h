#ifndef RIGID_BODY_H_
#define RIGID_BODY_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <math.h>

// Rigid Body used to be an integral part of the gameobject
// Commit id:

class RigidBody {
private:
	glm::vec3* pos;
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
	float mass;
	float dragCoefficient;

public:
	RigidBody(float _mass = 1.0f, float _dragCoefficient = 10.0f)
		: mass(_mass), dragCoefficient(_dragCoefficient) {}

	void setPos(glm::vec3* _pos) { pos = _pos; }

	void addForce(glm::vec3 _force) { force += _force; }

	void move(float dt);

private:


};

#endif // RIGID_BODY_H_
