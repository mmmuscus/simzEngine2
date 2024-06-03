#ifndef WINDOW_MANAGER_H_
#define WINDOW_MANAGER_H_

#include <GLFW/glfw3.h>

#include "resourceManagerGeneralIncludes.h"
#include "inputManager.h"

#include "../../vulkanWrappers/include/vulkanSurface.h"

class windowManager {
private:
	GLFWwindow* window;

	// Holding input
	inputManager input;

	static vulkanSurface* surface;

public:
	~windowManager();

	GLFWwindow* getWindow() { return window; }

	void setSurface(vulkanSurface* _surface) { surface = _surface; }

	void initWindow();

	void initGlfwInputHandling();
	static void framebufferResizeCallback(GLFWwindow* wndw, int height, int width);

	void checkIfWindowShouldClose();

private:

};

#endif // WINDOW_MANAGER_H_