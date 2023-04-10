#include "../include/inputManager.h"

mousePoint inputManager::mousePos = { WIDTH / 2.0f, HEIGHT / 2.0f };
mousePoint inputManager::mouseOffset = { 0, 0 };
bool inputManager::firstMouse = true;

bool inputManager::wKey = false;
bool inputManager::sKey = false;
bool inputManager::aKey = false;
bool inputManager::dKey = false;
bool inputManager::eKey = false;
bool inputManager::spaceKey = false;
bool inputManager::lShiftKey = false;
bool inputManager::escapeKey = false;

void inputManager::processKeyboardInput(GLFWwindow* window) {
	wKey = false;
	sKey = false;
	aKey = false;
	dKey = false;
	eKey = false;
	spaceKey = false;
	lShiftKey = false;
	escapeKey = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) wKey = true;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) sKey = true;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) aKey = true;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dKey = true;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) eKey = true;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) spaceKey = true;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) lShiftKey = true;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) escapeKey = true;
}

void inputManager::processMouseInput(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		mousePos.x = xpos;
		mousePos.y = ypos;
		firstMouse = false;
	}

	mouseOffset.x = mousePos.x - xpos;
	mouseOffset.y = mousePos.y - ypos;

	mousePos.x = xpos;
	mousePos.y = ypos;
}