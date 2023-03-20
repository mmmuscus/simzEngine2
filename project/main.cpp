#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include "vulkanWrappers/include/vulkanInstance.h"
#include "vulkanWrappers/include/vulkanSurface.h"
#include "vulkanWrappers/include/vulkanObject.h"
#include "vulkanWrappers/include/vulkanRenderPass.h"
#include "vulkanWrappers/include/vulkanRenderer.h"
#include "vulkanWrappers/include/vulkanModelData.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class Application {
public:
    void run() {
        initWindow();
        initVulkan();
        initImGui();
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
vulkanModelData modelData;

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
    surface.initImageViews(&instance);

    // Render Pass:
    renderPass.setDevice(instance.getDevice());
    renderPass.initRenderPass(&instance, surface.getFormat());

    // Descriptor sets:
    object.setDevice(instance.getDevice());
    object.initDescriptorSetLayout();

    // Pipeline:
    object.initPipeline(surface.getExtent(), renderPass.getRenderPass());

    // Depth resources:
    renderPass.initDepthResources(&instance, surface.getExtent());

    // Framebuffers:
    renderPass.initFrameBuffers(surface.getImageViews(), surface.getExtent());

    // CommandPool:
    instance.initCommandPool();

    // Texture:
    modelData.setDevice(instance.getDevice());
    modelData.initTextureImage(&instance);
    modelData.initTextureImageView(&instance);
    modelData.initTextureSampler(instance.getPhysicalDevice());

    // Vertex + Index Buffer:
    modelData.initVertexBuffer(&instance);
    modelData.initIndexBuffer(&instance);
    object.setModelData(&modelData);

    // Uniform Buffer:
    modelData.initUniformBuffers(&instance);

    // Descriptor Pool + Sets:
    object.initDescriptorPool();
    object.initDescriptorSets();

    // CommandBuffers:
    renderer.setDevice(instance.getDevice());
    renderer.initCommandBuffers(instance.getCommandPool());

    // SyncObjects:
    renderer.initSyncObjects();
}

void initImGui() {
}

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            renderer.drawFrame(
                &surface,
                &instance,
                &object,
                &renderPass
            );
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