#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include "vulkanWrappers/include/vkInstanceWrapper.h"
#include "vulkanWrappers/include/vkSurfaceWrapper.h"
#include "vulkanWrappers/include/vkDeviceWrapper.h"
#include "vulkanWrappers/include/vkSwapChainWrapper.h"
#include "vulkanWrappers/include/vkPipelineWrapper.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class Application {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    vkInstanceWrapper instance;
    vkSurfaceWrapper surface;
    vkDeviceWrapper device;
    vkSwapChainWrapper swapChain;
    vkPipelineWrapper graphicsPipeline;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        // creating instance
        instance.listExtensions();
        instance.init();
        instance.setupDebugMessenger();

        // creating window surface
        surface.init(instance.getInstance(), window);

        // getting devices
        device.init(instance.getInstance(), surface.getSurface());

        // creating swap chain
        swapChain.init(&device, window, surface.getSurface());

        // creating graphics pipeline
        // graphicsPipeline.init(device.getDevice(), swapChain.getImageFormat());
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    Application app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}