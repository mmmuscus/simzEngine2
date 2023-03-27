#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum direction {
	FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
};

// Default values
#define YAW		180.0f
#define PITCH	0.0f
#define SPEED	2.5f
#define SENS	0.1f
/*
const float YAW = 180.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENS = 0.1f;
*/

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
