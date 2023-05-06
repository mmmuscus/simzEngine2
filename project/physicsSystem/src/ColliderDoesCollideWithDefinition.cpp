#include "../include/Collider.h"

#include "../include/SphereCollider.h"
#include "../include/CapsuleCollider.h"

glm::vec3 Collider::doesCollideWith(Collider* other) {
	if (dynamic_cast<SphereCollider*>(other))
		return doesCollideWith(dynamic_cast<SphereCollider*>(other));

	if (dynamic_cast<CapsuleCollider*>(other))
		return doesCollideWith(dynamic_cast<CapsuleCollider*>(other));

	return glm::vec3(0.0f, 0.0f, 0.0f);
}