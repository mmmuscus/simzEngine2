#include "../include/gameObject.h"

void gameObject::update(size_t currentFrame) {
	if (transform != nullptr) {
		transform->calculateModelMatrix();
	}

	if (renderer != nullptr) {
		if (transform != nullptr) {
			renderer->updateModelTranslation(currentFrame, transform, objectNumber);
		}
		else {
			std::cout << "TRANSFORM IS NULL BUT RENDERER ISNT" << std::endl;
		}
	}
}