#include "../include/gameObject.h"

void gameObject::calculateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, pos);
	// Check if rotation matrix works as expected, lets do explicit 
	// ModelMat = S(cale) * R(otation) * T(ranslation):
	/*glm::mat4 rotMat = glm::mat4(1.0f);
	rotMat = glm::rotate(rotMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	rotMat = glm::rotate(rotMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	rotMat = glm::rotate(rotMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = modelMatrix * rotMat;*/

	// Creating quat from matrix and matrix from quat:
	glm::mat4 rotMat = glm::mat4(1.0f);
	qt.setFromEuler(rotation);
	rotMat = glm::rotate(rotMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	rotMat = glm::rotate(rotMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	rotMat = glm::rotate(rotMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix *= rotMat;
	/*qt.setFromMat(rotMat);
	glm::mat4 quatMat = qt.toMat();
	modelMatrix = modelMatrix * quatMat;*/

	// Method so far:
	/*modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));*/
	modelMatrix = glm::scale(modelMatrix, scale);
}

void gameObject::updateModelTranslation(uint32_t currentFrame) {
	modelData->getMeshData()->getUniformBuffer()->updateModelUniformBuffer(modelMatrix, currentFrame, objectNumber);
}