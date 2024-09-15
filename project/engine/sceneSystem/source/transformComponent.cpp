#include "../include/transformComponent.h"

void transformComponent::setOutsideRange(float y) {
	if (y >= M_PI / 2.0f || y <= -M_PI / 2.0f)
		outsideRange = true;
	else
		outsideRange = false;
}

glm::vec3 transformComponent::getEulerWithFlag() {
	return Quat::toEulerWithFlag(quaternion, outsideRange);
}

void transformComponent::calculateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, pos);
	modelMatrix *= Quat::toMat(quaternion);
	modelMatrix = glm::scale(modelMatrix, scale);
}