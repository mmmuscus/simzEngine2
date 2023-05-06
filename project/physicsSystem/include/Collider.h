#ifndef COLLIDER_H_
#define COLLIDER_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

class SphereCollider;
class CapsuleCollider;

class Collider {
protected:
	// Collision variables:
	glm::vec3* pos;
	float extent;

	// Model matrix:
	glm::mat4 modelMatrix;

public:

	glm::vec3* getPos() { return pos; }
	float getExtent() { return extent; }

	void setPos(glm::vec3* _pos) { pos = _pos; }
	void setModelMatrix(glm::mat4 _modelMatrix) { modelMatrix = _modelMatrix; }

	virtual void recalculateExtent() = 0;

	virtual void transformCollider() = 0;

	glm::vec3 doesCollideWith(Collider* other);
	virtual glm::vec3 doesCollideWith(SphereCollider* other) = 0;
	virtual glm::vec3 doesCollideWith(CapsuleCollider* other) = 0;

private:


};

#endif // COLLIDER_H_
