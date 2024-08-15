#include "../include/gameObject.h"

void gameObject::setOutsideRange(float y) {
	if (y >= M_PI / 2.0f || y <= -M_PI / 2.0f)
		outsideRange = true;
	else
		outsideRange = false;
}

glm::vec3 gameObject::getEuler() {
	return Quat::toEuler(quaternion);
}

glm::vec3 gameObject::getEulerWithFlag() {
	return Quat::toEulerWithFlag(quaternion, outsideRange);
}

void gameObject::calculateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, pos);

	modelMatrix *= Quat::toMat(quaternion);

	modelMatrix = glm::scale(modelMatrix, scale);
}

void gameObject::updateModelTranslation(uint32_t currentFrame) {
	modelData->getMeshData()->getUniformBuffer()->updateModelUniformBuffer(modelMatrix, currentFrame, objectNumber);
}