#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include "vulkanWrappers/include/vulkanInstance.h"
#include "vulkanWrappers/include/vulkanSurface.h"
#include "vulkanWrappers/include/vulkanObject.h"
#include "vulkanWrappers/include/vulkanRenderer.h"
#include "vulkanWrappers/include/vulkanDrawer.h"
#include "vulkanWrappers/include/vulkanModelData.h"
#include "vulkanWrappers/include/vulkanTextureData.h"

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
vulkanRenderer renderer;
vulkanDrawer drawer;
vulkanModelData modelData;
vulkanTextureData textureData;

void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->drawer.setFrameBufferResized(true);
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

    // Msaa Samples:
    renderer.initMsaaSamples(instance.getPhysicalDevice());

    // Depth resources:
    renderer.initDepthResources(&instance, surface.getExtent());

    // Render Pass:
    renderer.setDevice(instance.getDevice());
    renderer.initRenderPass(surface.getFormat());

    // Descriptor sets:
    object.setDevice(instance.getDevice());
    object.initDescriptorSetLayout();

    // Pipeline:
    object.initPipeline(surface.getExtent(), renderer.getRenderPass(), renderer.getMsaaSamples());

    // Color resources:
    renderer.initColorResources(&instance, surface.getFormat(), surface.getExtent());

    // Framebuffers:
    renderer.initFrameBuffers(surface.getImageViews(), surface.getExtent());

    // CommandPool:
    instance.initCommandPool();

    // Texture:
    textureData.setDevice(instance.getDevice());
    textureData.initTextureImage("textures/viking_room.png", &instance);
    textureData.initTextureImageView(&instance);
    textureData.initTextureSampler(instance.getPhysicalDevice());
    object.setTextureData(&textureData);

    // Model:
    modelData.loadModel("models/viking_room.obj");

    // Vertex + Index Buffer:
    modelData.setDevice(instance.getDevice());
    modelData.initVertexBuffer(&instance);
    modelData.initIndexBuffer(&instance);
    object.setModelData(&modelData);

    // Uniform Buffer:
    modelData.initUniformBuffers(&instance);

    // Descriptor Pool + Sets:
    object.initDescriptorPool();
    object.initDescriptorSets();

    // CommandBuffers:
    drawer.setDevice(instance.getDevice());
    drawer.initCommandBuffers(instance.getCommandPool());

    // SyncObjects:
    drawer.initSyncObjects();
}

void initImGui() {
    // ~~~Following:
    // https://vkguide.dev/docs/extra-chapter/implementing_imgui/
    // https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
    /*VkDescriptorPoolSize poolSizes[] =
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

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000;
    poolInfo.poolSizeCount = std::size(poolSizes);
    poolInfo.pPoolSizes = poolSizes;

    vk::DescriptorPool imguiPool;
    try {
        imguiPool = instance.getDevice().createDescriptorPool(poolInfo);
    } catch (vk::SystemError err)
    {
        throw std::runtime_error("failed to create ImGui descriptor pool!");
    }
    

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo info;
    info.Instance = instance.getInstance();
    info.PhysicalDevice = instance.getPhysicalDevice();
    info.Device = instance.getDevice();
    info.DescriptorPool = imguiPool;
    info.ImageCount = MAX_FRAMES_IN_FLIGHT;
    ImGui_ImplVulkan_Init(&info, renderPass.getRenderPass());

    vk::CommandBuffer commandBuffer = instance.beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    instance.endSingleTimeCommands(commandBuffer);

    instance.getDevice().waitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();*/
}

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            /*ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
            ImGui::Render();*/

            drawer.drawFrame(
                &surface,
                &instance,
                &object,
                &renderer
            );
        }

        instance.getDevice().waitIdle();
    }

    void cleanup() {
        /*ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();*/

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