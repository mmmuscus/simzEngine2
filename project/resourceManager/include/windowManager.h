#ifndef WINDOW_MANAGER_H_
#define WINDOW_MANAGER_H_

#include <GLFW/glfw3.h>

// default width and height
//#define WIDTH	800
//#define HEIGHT	600

class windowManager {
private:
	GLFWwindow* window;

public:
	void initWindow();

	bool frameBufferResizeCallback(int width, int height);
	void initGlfwInputHandling();

private:
};

#endif // WINDOW_MANAGER_H_