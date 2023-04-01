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

	/*switch (dir) {
		case FORWARD:	pos += forward * velocity;	break;
		case BACKWARD:	pos -= forward * velocity;	break;
		case RIGHT:		pos += right * velocity;	break;
		case LEFT:		pos -= right * velocity;	break;
		case UP:		pos += up * velocity;		break;
		case DOWN:		pos -= up * velocity;		break;
		default:		break;
	}*/

	if (input.getWKey())		pos += forward * velocity;
	if (input.getSKey())		pos -= forward * velocity;
	if (input.getDKey())		pos += right * velocity;
	if (input.getAKey())		pos -= right * velocity;
	if (input.getSpaceKey())	pos += up * velocity;
	if (input.getLShiftKey())	pos -= up * velocity;
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