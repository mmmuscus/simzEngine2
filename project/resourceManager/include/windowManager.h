#ifndef WINDOW_MANAGER_H_
#define WINDOW_MANAGER_H_

#include <GLFW/glfw3.h>

#include "resourceManagerGeneralIncludes.h"
#include "inputManager.h"

#include "../../vulkanWrappers/include/vulkanDrawer.h"

class windowManager {
private:
	GLFWwindow* window;

	// Holding input
	inputManager input;
	vulkanDrawer* drawer;

public:
	~windowManager();

	GLFWwindow* getWindow() { return window; }

	void initWindow();

	void initGlfwInputHandling();
	static void framebufferResizeCallback(GLFWwindow* wndw, int, int) {
		//framebufferResizeCallbackImpl(wndw);
	}

	void framebufferResizeCallbackImpl(GLFWwindow* wndw) {
		auto wnd = reinterpret_cast<windowManager*>(glfwGetWindowUserPointer(wndw));
		drawer->setFrameBufferResized(true);
	}

private:
};

#endif // WINDOW_MANAGER_H_