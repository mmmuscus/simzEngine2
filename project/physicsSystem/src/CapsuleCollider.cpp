#include "../include/CapsuleCollider.h"

#include "../include/CollisionUtilities.h"

glm::vec3 CapsuleCollider::doesCollideWith(SphereCollider* other) {
	return -Collisions::doCollision(other, this);
}

glm::vec3 CapsuleCollider::doesCollideWith(CapsuleCollider* other) {
	return Collisions::doCollision(this, other);
}