#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../resourceManager/include/inputManager.h"

enum direction {
	FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
};

// Default values
#define YAW		225.0f
#define PITCH	-30.0f
#define SPEED	2.5f
#define SENS	0.1f

class camera {
private:
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;

	float sens;
	float speed;

	// for moving around
	inputManager input;

public:
	camera(
		glm::vec3 _pos, glm::vec3 _worldUp,
		float _yaw = YAW, float _pitch = PITCH,
		float _speed = SPEED, float _sens = SENS
	);

	glm::mat4 getViewMatrix();

	void processKeyboard(direction dir, float deltaTime);
	void processMouseMovement(float x, float y);

private:
	void updateCameraVectors();

};

#endif // CAMERA_H_
