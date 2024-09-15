#include "../include/transform.h"

void transform::setOutsideRange(float y) {
	if (y >= M_PI / 2.0f || y <= -M_PI / 2.0f)
		outsideRange = true;
	else
		outsideRange = false;
}

glm::vec3 transform::getEulerWithFlag() {
	return Quat::toEulerWithFlag(quaternion, outsideRange);
}

void transform::calculateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, pos);
	modelMatrix *= Quat::toMat(quaternion);
	modelMatrix = glm::scale(modelMatrix, scale);
}