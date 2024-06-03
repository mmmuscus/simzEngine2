#ifndef COLLISION_UTILITIES_H_
#define COLLISION_UTILITIES_H_

#include "SphereCollider.h"
#include "CapsuleCollider.h"

#include <algorithm>

// https://wickedengine.net/2020/04/26/capsule-collision-detection/
namespace Collisions {
	glm::vec3 closestPointOnLineSegment(glm::vec3 A, glm::vec3 B, glm::vec3 point);
	bool doBoundingSpheresCollide(glm::vec3 A, glm::vec3 B, float rA, float rB);

	glm::vec3 doCollision(SphereCollider* ego, SphereCollider* other);
	glm::vec3 doCollision(SphereCollider* ego, CapsuleCollider* other);
	glm::vec3 doCollision(CapsuleCollider* ego, CapsuleCollider* other);
}

#endif // COLLISION_UTILITIES_H_
