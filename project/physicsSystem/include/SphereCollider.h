#ifndef SPHERE_COLLIDER_H_
#define SPHERE_COLLIDER_H_

#include "Collider.h"

class SphereCollider : public Collider {
private:
	float radius;

public:

	float getRadius() { return radius; }

	void recalculateExtent() { extent = radius; }

	bool doesCollideWith(SphereCollider& other) {
		if (glm::distance(*pos, *other.getPos()) > radius + other.getRadius())
			return false;

		return true;
	}

private:


};

#endif // SPHERE_COLLIDER_H_
