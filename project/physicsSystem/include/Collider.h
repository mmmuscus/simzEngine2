#ifndef COLLIDER_H_
#define COLLIDER_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>


class Collider {
private:
	// Collision variables:
	glm::vec3* pos;
	float extent;

	// Model matrix:
	glm::mat4* modelMatrix;

public:
	virtual void recalculateExtent() = 0;

private:


};

#endif // COLLIDER_H_
