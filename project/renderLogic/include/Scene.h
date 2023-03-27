#ifndef SCENE_H_
#define SCENE_H_

#include "camera.h"
#include "object.h"

#include <vector>

class scene {
private:
	camera* cam;
	object* obj;

public:
	void setCam(camera* _cam) { cam = _cam; }
	void setObj(object* _obj) { obj = _obj; }

	camera* getCam() { return cam; }
	object* getObj() { return obj; }

private:

};

#endif // SCENE_H_