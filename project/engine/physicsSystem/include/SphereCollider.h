#ifndef SPHERE_COLLIDER_H_
#define SPHERE_COLLIDER_H_

#include "Collider.h"

class SphereCollider : public Collider {
private:
	float radius;

public:
	SphereCollider(float _radius = 1.0f) : radius(_radius) {
		recalculateExtent();
	}

	float getRadius() { return radius; }

	void setRadius(float _radius) {
		radius = _radius;
		recalculateExtent();
	}

	void recalculateExtent() { extent = radius; }

	void transformCollider() { ; }

	glm::vec3 doesCollideWith(SphereCollider* other);
	glm::vec3 doesCollideWith(CapsuleCollider* other);

private:


};

#endif // SPHERE_COLLIDER_H_
