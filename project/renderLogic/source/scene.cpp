#include "../include/scene.h"

void scene::addObject(object* _object) {
	_object->setSceneTimer(sceneTimer);
	objects.push_back(_object);
}

void scene::init(
	vulkanInstance* instance,
	vk::DescriptorSetLayout descriptorsetLayout, vk::DescriptorPool descriptorPool,
	glm::vec3 cameraPos, glm::vec3 cameraWorldUp
) {
	sceneData = new vulkanSceneData();
	sceneData->init(instance, descriptorsetLayout, descriptorPool);
	cam = new camera(cameraPos, cameraWorldUp);
}

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
	sceneTimer->updateTime();

	for (size_t i = 0; i < objects.size(); i++) {
		objects[i]->updateTranslationVectors();
		objects[i]->calculateModelMatrix();
		objects[i]->updateModelTranslation(currentFrame);
	}
}