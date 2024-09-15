#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#define _USE_MATH_DEFINES

#include <glm/gtc/matrix_transform.hpp>

#include "../../general/include/Quat.h"

class transform {
private:
	// position, rotation, scale
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	Quat quaternion = Quat(0.0f, 0.0f, 0.0f, 1.0f);
	// Needed for converting Quaternion to Euler
	bool outsideRange = false;
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	
	// Model Matrix
	glm::mat4 modelMatrix = glm::mat4(1.0f);

public:
	transform() :
		pos(glm::vec3(0.0f, 0.0f, 0.0f)),
		quaternion(Quat(0.0f, 0.0f, 0.0f, 1.0f)),
		outsideRange(false),
		scale(glm::vec3(1.0f, 1.0f, 1.0f))
	{};
	transform(
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		Quat _quaternion = Quat(0.0f, 0.0f, 0.0f, 1.0f),
		bool _outsideRange = false, 
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		pos(_pos), quaternion(_quaternion), outsideRange(_outsideRange), scale(_scale)
	{};
	transform(
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 euler = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		pos(_pos), scale(_scale)
	{
		setOutsideRange(euler.y);
		quaternion = Quat::fromEuler(euler);
	}

	void setPos(glm::vec3 _pos) { pos = _pos; }
	void setQuaternion(Quat _quaternion) { quaternion = _quaternion.normalized(); }
	void setScale(glm::vec3 _scale) { scale = _scale; }

	glm::vec3 getPos() { return pos; }
	Quat getQuaternion() { return quaternion; }
	bool getOutsideRange() { return outsideRange; }
	glm::vec3 getScale() { return scale; }

	void setOutsideRange(float y);
	glm::vec3 getEulerWithFlag();

	void calculateModelMatrix();
private:
	
};

#endif // TRANSFORM_H_