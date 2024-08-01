#include "../include/gameObject.h"

void gameObject::calculateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, pos);

	quaternion = Quat::fromEuler(rotation);
	glm::mat4 rotMat = Quat::toMat(quaternion);
	modelMatrix *= rotMat;

	modelMatrix = glm::scale(modelMatrix, scale);
}

void gameObject::updateModelTranslation(uint32_t currentFrame) {
	modelData->getMeshData()->getUniformBuffer()->updateModelUniformBuffer(modelMatrix, currentFrame, objectNumber);
}