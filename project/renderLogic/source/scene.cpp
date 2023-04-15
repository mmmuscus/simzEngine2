#include "../include/scene.h"

void scene::resetSceneTimer() {
	sceneTimer = new timer();

	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->setSceneTimer(sceneTimer);
}

void scene::updateScene(size_t currentFrame, vk::Extent2D extent) {
	sceneData->updateSceneUniformBuffer(currentFrame, extent, cam->getViewMatrix());

	objects[0]->updateTranslationVectors();
	objects[0]->updateModelTranslation(currentFrame);
}