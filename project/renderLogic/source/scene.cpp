#include "../include/scene.h"

void scene::resetSceneTimer() {
	sceneTimer = new timer();

	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->setSceneTimer(sceneTimer);
}

void scene::defragmentObjectNumbers() {
	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->setObjectNumber(i);
}

void scene::updateScene(size_t currentFrame, vk::Extent2D extent) {
	sceneData->updateSceneUniformBuffer(currentFrame, extent, cam->getViewMatrix());

	for (size_t i = 0; i < objects.size(); i++) {
		objects[i]->updateTranslationVectors();
		objects[i]->updateModelTranslation(currentFrame);
	}
}