#include "../include/gameObject.h"

void gameObject::calculateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, pos);

	//glm::mat4 rotMat = glm::mat4(1.0f);
	//qt.setFromEuler(rotation);
	//rotMat = glm::rotate(rotMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	//rotMat = glm::rotate(rotMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	//rotMat = glm::rotate(rotMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	//modelMatrix *= rotMat;

	rotation = qt.toEuler();

	modelMatrix = glm::scale(modelMatrix, scale);
}

void gameObject::updateModelTranslation(uint32_t currentFrame) {
	modelData->getMeshData()->getUniformBuffer()->updateModelUniformBuffer(modelMatrix, currentFrame, objectNumber);
}