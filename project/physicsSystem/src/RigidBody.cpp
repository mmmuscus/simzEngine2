#include "../include/RigidBody.h"

const float EPSILON = 0.0001f;

void RigidBody::move(float dt) {
	glm::vec3 acceleration = force / mass;
	velocity += acceleration * dt;
	// Effect of drag:
	velocity *= expf(-dt * dragCoefficient / mass);
	*pos += velocity * dt;

	// Reset for next frame:
	force = glm::vec3(0.0f, 0.0f, 0.0f);
	if (fabs(velocity.x) < EPSILON) velocity.x = 0;
	if (fabs(velocity.y) < EPSILON) velocity.y = 0;
	if (fabs(velocity.z) < EPSILON) velocity.z = 0;
}