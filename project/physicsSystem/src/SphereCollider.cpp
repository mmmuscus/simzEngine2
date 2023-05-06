#include "../include/SphereCollider.h"

#include "../include/CollisionUtilities.h"

glm::vec3 SphereCollider::doesCollideWith(SphereCollider* other) {
	/*if (glm::distance(*pos, *other->getPos()) > radius + other->getRadius())
		return glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 res = glm::normalize(*pos - *other->getPos());
	float scale = radius + other->getRadius() - glm::distance(*pos, *other->getPos());

	return res *= scale;*/
	return Collisions::doCollision(this, other);
}

glm::vec3 SphereCollider::doesCollideWith(CapsuleCollider* other) {
	return Collisions::doCollision(this, other);
}