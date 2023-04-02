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

void camera::processKeyboard(float deltaTime) {
	float velocity = speed * deltaTime;

	if (input.getWKey())		pos += forward * velocity;
	if (input.getSKey())		pos -= forward * velocity;
	if (input.getDKey())		pos += right * velocity;
	if (input.getAKey())		pos -= right * velocity;
	if (input.getSpaceKey())	pos += up * velocity;
	if (input.getLShiftKey())	pos -= up * velocity;
}

void camera::processMouseMovement() {
	yaw += (input.getOffsetX() * sens);
	pitch += (input.getOffsetY() * sens);

	// maybe we could use clamp
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