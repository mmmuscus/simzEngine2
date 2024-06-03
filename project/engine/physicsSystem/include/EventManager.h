#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include "../../sceneSystem/include/gameObject.h"

const float FORCE_AMP = 100.0f;

struct CollisionEvent {
	gameObject* obj1;
	gameObject* obj2;
	glm::vec3 collisionVector;

	CollisionEvent(gameObject* _obj1, gameObject* _obj2, glm::vec3 _collisionVector
	) : obj1(_obj1), obj2(_obj2), collisionVector(_collisionVector) {}
};

class EventManager {
private:
	std::vector<CollisionEvent> collisionEvents;

public:
	void checkCollision(gameObject* obj1, gameObject* obj2) {
		if (obj1 == obj2)
			return;

		if (obj1->getCollider() == nullptr || obj2->getCollider() == nullptr)
			return;

		glm::vec3 collVect = obj1->getCollider()->doesCollideWith(obj2->getCollider());
		
		if (collVect != glm::vec3(0.0f, 0.0f, 0.0f))
			collisionEvents.push_back(CollisionEvent(obj1, obj2, collVect));
	}

	void resolveEvents() {
		for (size_t i = 0; i < collisionEvents.size(); i++) {
			collisionEvents[i].obj1->getRigidBody()->addForce(FORCE_AMP * collisionEvents[i].collisionVector);
			collisionEvents[i].obj2->getRigidBody()->addForce(FORCE_AMP * -collisionEvents[i].collisionVector);
		}

		collisionEvents.clear();
	}

private:
};

#endif // EVENT_MANAGER_H_