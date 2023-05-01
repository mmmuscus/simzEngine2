#ifndef SPHERE_COLLIDER_H_
#define SPHERE_COLLIDER_H_

#include "Collider.h"

class SphereCollider : public Collider {
private:
	float radius;

public:
	SphereCollider(
		glm::vec3* _pos,
		float _radius
	) {
		pos = _pos;
		radius = _radius;
		recalculateExtent();
	}

	float getRadius() { return radius; }

	void setRadius(float _radius) {
		radius = _radius;
		recalculateExtent();
	}

	void recalculateExtent() { extent = radius; }

	bool doesCollideWith(SphereCollider& other) {
		if (glm::distance(*pos, *other.getPos()) > radius + other.getRadius())
			return false;

		printf("Collision!\n");

		return true;
	}

private:


};

#endif // SPHERE_COLLIDER_H_
