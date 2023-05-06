#ifndef RIGID_BODY_H_
#define RIGID_BODY_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

class RigidBody {
private:
	glm::vec3* pos;
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
	float mass = 1.0f;
	float dragCoefficient = 0.1f;

public:
	RigidBody(float _mass = 1.0f, float _dragCoefficient = 0.1f)
		: mass(_mass), dragCoefficient(_dragCoefficient) {}

	void setPos(glm::vec3* _pos) { pos = _pos; }

	void addForce(glm::vec3 _force) { force += _force; }

	void addVelocity(glm::vec3 _velocity) { velocity += _velocity; }

	void move(float dt);

private:


};

#endif // RIGID_BODY_H_
