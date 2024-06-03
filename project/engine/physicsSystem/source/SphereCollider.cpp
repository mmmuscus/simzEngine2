#include "../include/SphereCollider.h"

#include "../include/CollisionUtilities.h"

glm::vec3 SphereCollider::doesCollideWith(SphereCollider* other) {
	return Collisions::doCollision(this, other);
}

glm::vec3 SphereCollider::doesCollideWith(CapsuleCollider* other) {
	return Collisions::doCollision(this, other);
}