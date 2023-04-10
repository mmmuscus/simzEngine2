#include "../include/scene.h"

void scene::resetSceneTimer() {
	sceneTimer = new timer();

	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->setSceneTimer(sceneTimer);
}