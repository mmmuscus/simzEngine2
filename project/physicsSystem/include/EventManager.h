#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include "../../renderLogic/include/object.h"

struct CollisionEvent {
	object* obj1;
	object* obj2;
	glm::vec3 collisionVector;

	CollisionEvent(object* _obj1, object* _obj2, glm::vec3 _collisionVector
	) : obj1(_obj1), obj2(_obj2), collisionVector(_collisionVector) {}
};

class EventManager {
private:
	std::vector<CollisionEvent> collisionEvents;

public:
	void checkCollision(object* obj1, object* obj2) {
		if (obj1 == obj2)
			return;

		glm::vec3 collVect = obj1->getCollider()->doesCollideWith(obj2->getCollider());
		
		if (collVect != glm::vec3(0.0f, 0.0f, 0.0f))
			collisionEvents.push_back(CollisionEvent(obj1, obj2, collVect));
	}

	void resolveEvents() {
		for (size_t i = 0; i < collisionEvents.size(); i++) {
			printf(
				"Object %d, collides with object %d with vector: (%f, %f, %f)\n",
				collisionEvents[i].obj1->getObjectNumber(),
				collisionEvents[i].obj2->getObjectNumber(),
				collisionEvents[i].collisionVector.x,
				collisionEvents[i].collisionVector.y,
				collisionEvents[i].collisionVector.z
			);

			collisionEvents[i].obj1->addVelocity(collisionEvents[i].collisionVector);
		}

		collisionEvents.clear();
	}

private:
};

#endif // EVENT_MANAGER_H_