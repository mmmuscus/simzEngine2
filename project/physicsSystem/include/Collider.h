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
	glm::vec3 center;
	float extent;

public:
	virtual void recalculateExtent() = 0;

private:


};

#endif // COLLIDER_H_
