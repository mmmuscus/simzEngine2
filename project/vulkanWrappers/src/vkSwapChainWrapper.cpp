#include "../include/vkSwapChainWrapper.h"

vkSwapChainWrapper::~vkSwapChainWrapper() {
    vkDestroySurfaceKHR(*instance, *surface, nullptr);
}

void vkSwapChainWrapper::initSurface(VkInstance* _instance, GLFWwindow* window) {
    surface = new VkSurfaceKHR();
    
    if (glfwCreateWindowSurface(*_instance, window, nullptr, surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");

    instance = _instance;
}