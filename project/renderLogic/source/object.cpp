#include "../include/object.h"

const float EPSILON = 0.0001f;
const float VELOCITY_AMP = 100.0f;

// Assumes that the sceneTimer was already updated.
void object::updateTranslationVectors() {
	pos += velocity * sceneTimer->getDeltaTime() * VELOCITY_AMP;
	rotation = rotation;
	scale = scale;

	velocity *= dampening;
	if (fabs(velocity.x) < EPSILON) velocity.x = 0;
	if (fabs(velocity.y) < EPSILON) velocity.y = 0;
	if (fabs(velocity.z) < EPSILON) velocity.z = 0;
}

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