#ifndef CAPSULE_COLLIDER_H_
#define CAPSULE_COLLIDER_H_

#include "Collider.h"

class CapsuleCollider : public Collider {
private:
	glm::vec3 base;
	// Maybe we dont need this cos tip = base mirrored on pos
	// glm::vec3 tip;
	float radius;

	// Transformed points
	glm::vec3 transformedBase;

public:
	CapsuleCollider(glm::vec3 _base, float _radius = 1.0f) {
		base = _base;
		radius = _radius;
		recalculateExtent();
	}

	void setBase(glm::vec3 _base) {
		base = _base;
		recalculateExtent();
	}
	void setRadius(float _radius) {
		radius = _radius;
		recalculateExtent();
	}

	void recalculateExtent() { extent = glm::length(base) + radius; }

	void transformCollider() {
		transformedBase = glm::vec3(modelMatrix * glm::vec4(base, 1.0f));
	}

private:


};

#endif // CAPSULE_COLLIDER_H_