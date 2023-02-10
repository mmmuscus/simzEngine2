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
#include "vulkanWrappers/include/vkCommandWrapper.h"
#include "vulkanWrappers/include/vkSyncWrapper.h"

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
    vkCommandWrapper command;
    vkSyncWrapper syncObject;

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
        vkWaitForFences(*device.getDevice(), 1, syncObject.getInFlightFence(), VK_TRUE, UINT64_MAX);
        vkResetFences(*device.getDevice(), 1, syncObject.getInFlightFence());

        uint32_t imageIndex;
        vkAcquireNextImageKHR(
            *device.getDevice(),
            *swapChain.getSwapChain(),
            UINT64_MAX,
            *syncObject.getImageAvailableSemaphore(),
            VK_NULL_HANDLE,
            &imageIndex);

        vkResetCommandBuffer(*command.getCommandBuffer(), 0);
        command.recordCommandBuffer(&graphicsPipeline, &swapChain, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { *syncObject.getImageAvailableSemaphore() };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = command.getCommandBuffer();

        VkSemaphore signalSemaphores[] = { *syncObject.getRenderFinishedSemaphore() };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(*device.getGraphicsQueue(), 1, &submitInfo, *syncObject.getInFlightFence()) != VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { *swapChain.getSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(*device.getPresentQueue(), &presentInfo);
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