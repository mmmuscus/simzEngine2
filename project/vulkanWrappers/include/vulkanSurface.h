#ifndef VULKAN_SURFACE_H_
#define VULKAN_SURFACE_H_

#include "vulkanIncludes.h"

#include "vulkanInstance.h"

class vulkanSurface {
private:
	vk::UniqueSurfaceKHR surface;



	vk::Instance instance;

public:
	void init(vk::Instance _instance);

private:
	void initSurface();
};

#endif // VULKAN_SURFACE_H_