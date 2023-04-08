#include "../include/object.h"

void object::updateTranslationVectors() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	pos = pos;
	rotation.x = time * glm::radians(90.0f);
	scale = scale;
}

void object::updateModelTranslation(uint32_t currentFrame) {
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, pos);
	trans = glm::rotate(trans, rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, scale);

	modelData->updateModelUniformBuffer(trans, currentFrame);
}