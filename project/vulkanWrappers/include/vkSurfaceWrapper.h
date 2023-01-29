#ifndef VK_WINDOW_SURFACE_WRAPPER_H_
#define VK_WINDOW_SURFACE_WRAPPER_H_

#include "generalIncludes.h"

class vkSurfaceWrapper {
private:
	VkSurfaceKHR* surface;

	VkInstance* instance;

public:
	vkSurfaceWrapper() : surface(nullptr), instance(nullptr) {}
	~vkSurfaceWrapper();

	VkSurfaceKHR* getSurface() { return surface; }

	void init(VkInstance* _instance, GLFWwindow* window);
};

#endif // VK_WINDOW_SURFACE_WRAPPER_H_