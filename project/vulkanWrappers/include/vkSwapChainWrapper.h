#ifndef VK_WINDOW_SURFACE_WRAPPER_H_
#define VK_WINDOW_SURFACE_WRAPPER_H_

#include "generalIncludes.h"

class vkSwapChainWrapper {
private:
	VkSurfaceKHR* surface;

	VkInstance* instance;

public:
	vkSwapChainWrapper() : surface(nullptr), instance(nullptr) {}
	~vkSwapChainWrapper();

	VkSurfaceKHR* getSurface() { return surface; }

	void initSurface(VkInstance* _instance, GLFWwindow* window);
};

#endif // VK_WINDOW_SURFACE_WRAPPER_H_