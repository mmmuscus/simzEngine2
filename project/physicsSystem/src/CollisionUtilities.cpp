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

	return res * scale;
}

glm::vec3 Collisions::doCollision(SphereCollider* ego, CapsuleCollider* other) {
	if (doBoundingSpheresCollide(*ego->getPos(), *other->getPos(), ego->getRadius(), other->getExtent()))
		return glm::vec3(0.0f, 0.0f, 0.0f);

	other->transformCollider();

	glm::vec3 closest = closestPointOnLineSegment(
		other->getTransformedBase(),
		other->getTransformedTip(),
		*ego->getPos()
	);

	float scale = ego->getRadius() + other->getRadius() - glm::distance(*ego->getPos(), closest);
	if (scale < 0.0f)
		return glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 res = glm::normalize(*ego->getPos() - closest);
	return res * scale;
}

// https://wickedengine.net/2020/04/26/capsule-collision-detection/
glm::vec3 Collisions::doCollision(CapsuleCollider* ego, CapsuleCollider* other) {
	if (doBoundingSpheresCollide(*ego->getPos(), *other->getPos(), ego->getExtent(), other->getExtent()))
		return glm::vec3(0.0f, 0.0f, 0.0f);

	ego->transformCollider();
	other->transformCollider();

	// a_A -> ego_Base, a_B -> ego_Tip
	// b_A -> other_Base, b_B -> other_Tip
	glm::vec3 v0 = other->getTransformedBase() - ego->getTransformedBase();
	glm::vec3 v1 = other->getTransformedTip() - ego->getTransformedBase();
	glm::vec3 v2 = other->getTransformedBase() - ego->getTransformedTip();
	glm::vec3 v3 = other->getTransformedTip() - ego->getTransformedTip();

	float d0 = glm::dot(v0, v0);
	float d1 = glm::dot(v1, v1);
	float d2 = glm::dot(v2, v2);
	float d3 = glm::dot(v3, v3);

	glm::vec3 closestEgo;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
		closestEgo = ego->getTransformedTip();
	else
		closestEgo = ego->getTransformedBase();

	glm::vec3 closestOther = closestPointOnLineSegment(
		other->getTransformedBase(),
		other->getTransformedTip(),
		closestEgo
	);

	closestEgo = closestPointOnLineSegment(
		ego->getTransformedBase(),
		ego->getTransformedTip(),
		closestOther
	);

	float scale = ego->getRadius() + other->getRadius() - glm::distance(closestEgo, closestOther);
	if (scale < 0.0f)
		return glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 res = glm::normalize(closestEgo - closestOther);
	return res * scale;
}