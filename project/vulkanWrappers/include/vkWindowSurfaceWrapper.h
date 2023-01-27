#ifndef VK_WINDOW_SURFACE_WRAPPER_H_
#define VK_WINDOW_SURFACE_WRAPPER_H_

#include "generalIncludes.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class vkWindowSurfaceWrapper {
private:
	VkSurfaceKHR* surface;

	VkInstance* instance;

public:
	vkWindowSurfaceWrapper() : surface(nullptr), instance(nullptr) {}
	~vkWindowSurfaceWrapper();

	VkSurfaceKHR* getSurface() { return surface; }

	void init(VkInstance* _instance, GLFWwindow* window);
};

#endif // VK_WINDOW_SURFACE_WRAPPER_H_