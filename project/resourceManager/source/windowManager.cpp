#include "../include/windowManager.h"

vulkanDrawer* windowManager::drawer = nullptr;

windowManager::~windowManager() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void windowManager::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void windowManager::initGlfwInputHandling() {
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, input.processMouseInput);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}