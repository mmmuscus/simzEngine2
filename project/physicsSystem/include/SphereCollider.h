#ifndef SPHERE_COLLIDER_H_
#define SPHERE_COLLIDER_H_

#include "Collider.h"

class SphereCollider : public Collider {
private:
	float radius;

public:
	SphereCollider(float _radius = 1.0f) {
		radius = _radius;
		recalculateExtent();

		rotateable = false;
	}

	float getRadius() { return radius; }

	void setRadius(float _radius) {
		radius = _radius;
		recalculateExtent();
	}

	void recalculateExtent() { extent = radius; }

	glm::vec3 doesCollideWith(SphereCollider* other) {
		if (glm::distance(*pos, *other->getPos()) > radius + other->getRadius())
			return glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec3 res = glm::normalize(*pos - *other->getPos());
		float scale = radius + other->getRadius() - glm::distance(*pos, *other->getPos());
		res *= scale;

		return res;
	}

private:


};

#endif // SPHERE_COLLIDER_H_
