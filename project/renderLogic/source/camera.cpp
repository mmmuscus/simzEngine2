#include "../include/camera.h"

camera::camera(
	glm::vec3 _pos, glm::vec3 _worldUp,
	float _yaw, float _pitch,
	float _speed, float _sens
) :
	pos(_pos), worldUp(_worldUp),
	yaw(_yaw), pitch(_pitch),
	speed(_speed), sens(_sens)
{
	updateCameraVectors();
}

glm::mat4 camera::getViewMatrix() {
	return glm::lookAt(pos, pos + forward, up);
}

void camera::processKeyboard(direction dir, float deltaTime) {
	float velocity = speed * deltaTime;

	switch (dir) {
		FORWARD:	pos += forward * velocity;
		BACKWARD:	pos -= forward * velocity;
		RIGHT:		pos += right * velocity;
		LEFT:		pos -= right * velocity;
		UP:			pos += up * velocity;
		DOWN:		pos -= up * velocity;
		default:	break;
	}
}

void camera::processMouseMovement(float x, float y) {
	x *= sens;
	y *= sens;

	yaw += x;
	pitch += y;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	updateCameraVectors();
}

void camera::updateCameraVectors() {
	glm::vec3 newForward;

	newForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newForward.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	newForward.z = sin(glm::radians(pitch));

	forward = glm::normalize(newForward);

	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));
}