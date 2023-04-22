#include "../include/object.h"

void object::updateTranslationVectors() {
	sceneTimer->updateTime();

	pos = pos;
	rotation.z = sceneTimer->getTime() * glm::radians(90.0f);
	scale = scale;
}

void object::updateModelTranslation(uint32_t currentFrame) {
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, pos);
	trans = glm::rotate(trans, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, scale);

	modelData->getMeshData()->getUniformBuffer()->updateModelUniformBuffer(trans, currentFrame, objectNumber);
}