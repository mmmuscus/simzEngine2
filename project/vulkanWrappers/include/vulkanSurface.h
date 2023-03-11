#ifndef VULKAN_SURFACE_H_
#define VULKAN_SURFACE_H_

#include "vulkanIncludes.h"

#include "vulkanInstance.h"

class vulkanSurface {
private:
	vk::SurfaceKHR surface;


	// Not maintained by the class:
	vk::Instance instance;

public:
	~vulkanSurface();

	void setInstance(vk::Instance _instance) { instance = _instance; }

	vk::SurfaceKHR getSurface() { return surface; }

	void initSurface(GLFWwindow* window);

private:
};

#endif // VULKAN_SURFACE_H_