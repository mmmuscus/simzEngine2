#include "../include/rendererComponent.h"

void rendererComponent::updateModelTranslation(
	uint32_t currentFrame, transformComponent* transform, uint32_t objectNumber
	) {
	if (transform == nullptr)
		return;
	
	modelData->getMeshData()->getUniformBuffer()->updateModelUniformBuffer(
		transform->getModelMatrix(), currentFrame, objectNumber
	);
}