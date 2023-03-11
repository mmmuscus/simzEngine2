#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

/*#include "vulkanWrappers/include/vkInstanceWrapper.h"
#include "vulkanWrappers/include/vkSurfaceWrapper.h"
#include "vulkanWrappers/include/vkDeviceWrapper.h"
#include "vulkanWrappers/include/vkSwapChainWrapper.h"
#include "vulkanWrappers/include/vkPipelineWrapper.h"
#include "vulkanWrappers/include/vkCommandWrapper.h"
#include "vulkanWrappers/include/vkSyncWrapper.h"*/

#include "vulkanWrappers/include/vulkanInstance.h"
#include "vulkanWrappers/include/vulkanSurface.h"
#include "vulkanWrappers/include/vulkanObject.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

/*class Application {
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
    vkCommandWrapper command;
    vkSyncWrapper syncObject;

    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
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
        graphicsPipeline.init(device.getDevice(), swapChain.getImageFormat());

        // creating framebuffers
        swapChain.initFrameBuffers(graphicsPipeline.getRenderPass());

        // creating command pool and command buffer
        command.init(&device);

        // creating sync objects
        syncObject.init(device.getDevice());
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(*device.getDevice());
    }
    
    void drawFrame() {
        VkFence fence = syncObject.getInFlightFence(currentFrame);
        VkCommandBuffer buffer = command.getCommandBuffer(currentFrame);

        vkWaitForFences(*device.getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(
            *device.getDevice(),
            *swapChain.getSwapChain(),
            UINT64_MAX,
            syncObject.getImageAvailableSemaphore(currentFrame),
            VK_NULL_HANDLE,
            &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            swapChain.recreateSwapChain(
                &device,
                window, 
                surface.getSurface(), 
                graphicsPipeline.getRenderPass());
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // Only reset the fence if we are submitting work
        vkResetFences(*device.getDevice(), 1, &fence);

        vkResetCommandBuffer(command.getCommandBuffer(currentFrame), 0);
        command.recordCommandBuffer(command.getCommandBuffer(currentFrame), &graphicsPipeline, &swapChain, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { syncObject.getImageAvailableSemaphore(currentFrame) };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffer;

        VkSemaphore signalSemaphores[] = { syncObject.getRenderFinishedSemaphore(currentFrame) };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(*device.getGraphicsQueue(), 1, &submitInfo, syncObject.getInFlightFence(currentFrame)) != VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { *swapChain.getSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(*device.getPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            swapChain.recreateSwapChain(
                &device,
                window,
                surface.getSurface(),
                graphicsPipeline.getRenderPass());
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void cleanup() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};*/

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

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        instance.listExtensions();

        // Instance:
        instance.initInstance();
        instance.initCallback();

        // Surface:
        surface.setInstance(instance.getInstance());
        surface.initSurface(window);

        // Devices:
        instance.setSurface(surface.getSurface());
        instance.initPhysicalDevice();
        instance.initDevice();

        // SwapChain:
        surface.setDevice(instance.getDevice());
        surface.initSwapChain(&instance, window);
        surface.initImageViews();

        // Render Pass:

        // Pipeline:
        object.setDevice(instance.getDevice());
        object.initPipeline(surface.getExtent());
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