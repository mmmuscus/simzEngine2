#include "../include/Collider.h"

#include "../include/SphereCollider.h"

glm::vec3 Collider::doesCollideWith(Collider* other) {
	if (dynamic_cast<SphereCollider*>(other))
		return doesCollideWith(dynamic_cast<SphereCollider*>(other));
}