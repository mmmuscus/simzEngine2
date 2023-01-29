#include "../include/vkSurfaceWrapper.h"

vkSurfaceWrapper::~vkSurfaceWrapper() {
	vkDestroySurfaceKHR(*instance, *surface, nullptr);
}

void vkSurfaceWrapper::init(VkInstance* _instance, GLFWwindow* window) {
    surface = new VkSurfaceKHR();

    if (glfwCreateWindowSurface(*_instance, window, nullptr, surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");

    instance = _instance;
}