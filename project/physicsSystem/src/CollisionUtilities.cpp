#include "../include/CollisionUtilities.h"

glm::vec3 Collisions::closestPointOnLineSegment(glm::vec3 A, glm::vec3 B, glm::vec3 point) {
	glm::vec3 AB = B - A;
	float t = glm::dot(point - A, AB) / dot(AB, AB);
	return A + std::min(std::max(t, 0.0f), 1.0f) * AB;
}

bool Collisions::doBoundingSpheresCollide(glm::vec3 A, glm::vec3 B, float rA, float rB) {
	return glm::distance(A, B) > rA + rB;
}

glm::vec3 Collisions::doCollision(SphereCollider* ego, SphereCollider* other) {
	if (doBoundingSpheresCollide(*ego->getPos(), *other->getPos(), ego->getRadius(), other->getRadius()))
		return glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 res = glm::normalize(*ego->getPos() - *other->getPos());
	float scale = ego->getRadius() + other->getRadius() - glm::distance(*ego->getPos(), *other->getPos());

	return res *= scale;
}

glm::vec3 Collisions::doCollision(SphereCollider* ego, CapsuleCollider* other) {
	if (doBoundingSpheresCollide(*ego->getPos(), *other->getPos(), ego->getRadius(), other->getExtent()))
		return glm::vec3(1.0f, 1.0f, 1.0f);

	other->transformCollider();

	glm::vec3 closest = closestPointOnLineSegment(
		other->getTransformedBase(),
		other->getTransformedTip(),
		*ego->getPos()
	);

	glm::vec3 res = glm::normalize(*ego->getPos() - closest);
	float scale = ego->getRadius() + other->getRadius() - glm::distance(*ego->getPos(), closest);

	return res *= scale;
}

glm::vec3 Collisions::doCollision(CapsuleCollider* ego, CapsuleCollider* other) {
	return glm::vec3(0.0f, 0.0f, 0.0f);
}