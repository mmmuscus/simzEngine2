#ifndef SCENE_H_
#define SCENE_H_

#include "../../general/include/timer.h"

#include "../../vulkanWrappers/include/vulkanSceneData.h"

#include "camera.h"
#include "gameObject.h"

#include <vector>

class scene {
private:
	vulkanSceneData* sceneData;

	camera* cam;
	std::vector<gameObject*> gameObjects;

	// timer
	timer* sceneTimer;

public:
	void destroy();

	void setSceneData(vulkanSceneData* _sceneData) { sceneData = _sceneData; }
	void setCam(camera* _cam) { cam = _cam; }

	void addObject(gameObject* _object);

	vulkanSceneData* getSceneData() { return sceneData; }
	camera* getCam() { return cam; }

	std::vector<gameObject*> getObjects() { return gameObjects; }

	void init(
		vulkanInstance* instance, 
		vk::DescriptorSetLayout descriptorsetLayout, vk::DescriptorPool descriptorPool,
		glm::vec3 cameraPos = glm::vec3(2.0f, 2.0f, 2.0f),
		glm::vec3 cameraWorldUp = glm::vec3(0.0f, 0.0f, 1.0f)
	);

	void resetSceneTimer();
	void defragmentObjectNumbers();

	void updateScene(size_t currentFrame, vk::Extent2D extent);

private:

};

#endif // SCENE_H_