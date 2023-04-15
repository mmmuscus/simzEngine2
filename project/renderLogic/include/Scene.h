#ifndef SCENE_H_
#define SCENE_H_

#include "../../general/include/timer.h"

#include "../../vulkanWrappers/include/vulkanSceneData.h"

#include "camera.h"
#include "object.h"

#include <vector>

class scene {
private:
	vulkanSceneData* sceneData;

	camera* cam;
	std::vector<object*> objects;

	// timer
	timer* sceneTimer;

public:
	void setSceneData(vulkanSceneData* _sceneData) { sceneData = _sceneData; }
	void setCam(camera* _cam) { cam = _cam; }

	void addObject(object* _object) { objects.push_back(_object); }

	vulkanSceneData* getSceneData() { return sceneData; }
	camera* getCam() { return cam; }

	std::vector<object*> getObjects() { return objects; }

	void resetSceneTimer();

	void updateScene(size_t currentFrame, vk::Extent2D extent);

private:

};

#endif // SCENE_H_