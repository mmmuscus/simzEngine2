#include "../include/scene.h"

void scene::destroy() {
	sceneData->destroy();
}

void scene::addObject(gameObject* _object) {
	_object->setSceneTimer(sceneTimer);
	gameObjects.push_back(_object);
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

	for (size_t i = 0; i < gameObjects.size(); i++)
		gameObjects[i]->setSceneTimer(sceneTimer);
}

void scene::defragmentObjectNumbers() {
	for (size_t i = 0; i < gameObjects.size(); i++)
		// 0 is reserved for init value
		gameObjects[i]->setObjectNumber(i + 1);
}

void scene::updateScene(size_t currentFrame, vk::Extent2D extent) {
	sceneData->updateSceneUniformBuffer(currentFrame, extent, cam->getViewMatrix());
	sceneTimer->updateTime();

	for (size_t i = 0; i < gameObjects.size(); i++) {
		// Here is where the physics used to move the game objects
		gameObjects[i]->update(currentFrame);
	}
}