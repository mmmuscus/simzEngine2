#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include "vulkanWrappers/include/vulkanInstance.h"
#include "vulkanWrappers/include/vulkanSurface.h"
#include "vulkanWrappers/include/vulkanObject.h"
#include "vulkanWrappers/include/vulkanRenderPass.h"
#include "vulkanWrappers/include/vulkanRenderer.h"

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

    vulkanInstance instance;
    vulkanSurface surface;
    vulkanObject object;
    vulkanRenderPass renderPass;
    vulkanRenderer renderer;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        app->renderer.setFrameBufferResized(true);
    }

    void initVulkan() {
        instance.listExtensions();

        // Instance:
        instance.initInstance();
        instance.initCallback();

        // Surface:
        surface.setWindow(window);
        surface.setInstance(instance.getInstance());
        surface.initSurface();

        // Devices:
        instance.setSurface(surface.getSurface());
        instance.initPhysicalDevice();
        instance.initDevice();

        // SwapChain:
        surface.setDevice(instance.getDevice());
        surface.initSwapChain(&instance);
        surface.initImageViews();

        // Render Pass:
        renderPass.setDevice(instance.getDevice());
        renderPass.initRenderPass(surface.getFormat());

        // Pipeline:
        object.setDevice(instance.getDevice());
        object.initPipeline(surface.getExtent(), renderPass.getRenderPass());

        // Framebuffers:
        renderPass.initFrameBuffers(surface.getImageViews(), surface.getExtent());

        // CommandPool + CommandBuffers:
        renderer.setDevice(instance.getDevice());
        renderer.initCommandPool(&instance);
        renderer.initCommandBuffers();

        // Vertex Buffer:
        object.initVertexBuffer(&instance, renderer.getCommandPool());

        // SyncObjects:
        renderer.initSyncObjects();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            renderer.drawFrame(
                &surface,
                &instance,
                &object,
                &renderPass);
        }

        instance.getDevice().waitIdle();
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