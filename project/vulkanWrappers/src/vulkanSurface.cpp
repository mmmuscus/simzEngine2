#include "../include/vulkanSurface.h"

vulkanSurface::~vulkanSurface() {
	instance.destroySurfaceKHR(surface);
}

void vulkanSurface::initSurface(GLFWwindow* window) {
	VkSurfaceKHR surf;

	if (glfwCreateWindowSurface(instance, window, nullptr, &surf) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");

	surface = surf;
}