#ifndef COLLIDER_H_
#define COLLIDER_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

class Collider {
protected:
	// Collision variables:
	glm::vec3* pos;
	glm::vec3* rotation;
	float extent;

	// Model matrix:
	glm::mat4 modelMatrix;

public:

	glm::vec3* getPos() { return pos; }

	virtual void recalculateExtent() = 0;

	void calculateModelMatrix() {
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, *pos);
		modelMatrix = glm::rotate(modelMatrix, rotation->x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, rotation->y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, rotation->z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

private:


};

#endif // COLLIDER_H_
