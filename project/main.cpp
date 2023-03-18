#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.cpp>
#include <imgui_impl_vulkan.h>

#include "vulkanWrappers/include/generalIncludes.h"

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

    // Vertex + Index Buffer:
    modelData.setDevice(instance.getDevice());
    modelData.initVertexBuffer(&instance, renderer.getCommandPool());
    modelData.initIndexBuffer(&instance, renderer.getCommandPool());
    object.setModelData(&modelData);

    // SyncObjects:
    renderer.initSyncObjects();
}

void initImGui() {
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;

    if (vkCreateDescriptorPool(instance.getDevice(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance.getInstance();
    init_info.PhysicalDevice = instance.getPhysicalDevice();
    init_info.Device = instance.getDevice();
    init_info.Queue = instance.getGraphicsQueue();
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGui_ImplVulkan_Init(&init_info, renderPass.getRenderPass());

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