#include "../include/RigidBody.h"

const float EPSILON = 0.0001f;
const float VELOCITY_AMP = 100.0f;

void RigidBody::move(float dt) {
	//glm::vec3 acceleration = force / mass;
	*pos += velocity * dt * VELOCITY_AMP;

	velocity *= glm::vec3(0.95f, 0.95f, 0.95f);
	if (fabs(velocity.x) < EPSILON) velocity.x = 0;
	if (fabs(velocity.y) < EPSILON) velocity.y = 0;
	if (fabs(velocity.z) < EPSILON) velocity.z = 0;
}