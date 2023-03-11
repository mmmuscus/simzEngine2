#ifndef VK_WINDOW_SURFACE_WRAPPER_H_
#define VK_WINDOW_SURFACE_WRAPPER_H_

#include "generalIncludes.h"

class vkSurfaceWrapper {
private:
	VkSurfaceKHR* surface;

	// The following resources are not being maintained by the class
	VkInstance* instance;

public:
	vkSurfaceWrapper() : surface(nullptr), instance(nullptr) {}
	~vkSurfaceWrapper();

	VkSurfaceKHR* getSurfacePtr() { return surface; }

	void init(VkInstance* _instance, GLFWwindow* window);
};

#endif // VK_WINDOW_SURFACE_WRAPPER_H_