#ifndef SCENE_H_
#define SCENE_H_

#include "camera.h"
#include "object.h"

#include <vector>

class scene {
private:
	camera* cam;
	std::vector<object*> objects;

public:
	void setCam(camera* _cam) { cam = _cam; }

	void addObject(object* _object) { objects.push_back(_object); }

	camera* getCam() { return cam; }

	std::vector<object*> getObjects() { return objects; }

private:

};

#endif // SCENE_H_