#include "../include/gameObject.h"

void gameObject::calculateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, pos);

	qt.setFromEuler(rotation);
	glm::mat4 rotMat = qt.toMat();
	modelMatrix *= rotMat;

	modelMatrix = glm::scale(modelMatrix, scale);
}

void gameObject::updateModelTranslation(uint32_t currentFrame) {
	modelData->getMeshData()->getUniformBuffer()->updateModelUniformBuffer(modelMatrix, currentFrame, objectNumber);
}