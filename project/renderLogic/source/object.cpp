#include "../include/object.h"

void object::calculateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, pos);
	modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	setColliderModelMat();
	modelMatrix = glm::scale(modelMatrix, scale);
}

void object::updateModelTranslation(uint32_t currentFrame) {
	modelData->getMeshData()->getUniformBuffer()->updateModelUniformBuffer(modelMatrix, currentFrame, objectNumber);
}