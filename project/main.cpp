#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "general/include/timer.h"

#include "resourceManager/include/windowManager.h"
#include "resourceManager/include/inputManager.h"

#include "vulkanWrappers/include/vulkanInstance.h"
#include "vulkanWrappers/include/vulkanSurface.h"
#include "vulkanWrappers/include/vulkanObject.h"
#include "vulkanWrappers/include/vulkanRenderer.h"
#include "vulkanWrappers/include/vulkanDrawer.h"
#include "vulkanWrappers/include/vulkanModelData.h"
#include "vulkanWrappers/include/vulkanTextureData.h"
#include "vulkanWrappers/include/vulkanSceneData.h"

#include "renderLogic/include/scene.h"
#include "renderLogic/include/object.h"
#include "renderLogic/include/camera.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// for the camera:
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0;

scene mainScene;
inputManager input;
bool frstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (frstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        frstMouse = false;
    }

    // reversed cos windows funykness(?)
    // TODO: look into this
    float xOffset = lastX - xpos;
    float yOffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    mainScene.getCam()->processMouseMovement(xOffset, yOffset);
}

void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    camera* cam = mainScene.getCam();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam->processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam->processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam->processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam->processKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cam->processKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cam->processKeyboard(DOWN, deltaTime);
}

class Application {
public:
    void run() {
        initWindow();
        initVulkan();
        initGlfwInputHandling();
        // initImGui();
        initScene();
        mainLoop();
        cleanup();
    }

private:
GLFWwindow* window;

vulkanInstance instance;
vulkanSurface surface;
vulkanObject obj;
vulkanRenderer renderer;
vulkanDrawer drawer;
vulkanModelData modelData;
vulkanTextureData textureData;
vulkanSceneData sceneData;

object demoObj;
camera cam = camera(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));

// for input processing:
timer inputTimer;

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

void initGlfwInputHandling() {
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    obj.setDevice(instance.getDevice());
    obj.initDescriptorSetLayout();

    // Pipeline:
    obj.initPipeline(surface.getExtent(), renderer.getRenderPass(), renderer.getMsaaSamples());

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

    // Model:
    modelData.loadModel("models/viking_room.objj");

    // Vertex + Index Buffer:
    modelData.setDevice(instance.getDevice());
    modelData.initVertexBuffer(&instance);
    modelData.initIndexBuffer(&instance);

    // Uniform Buffer:
    modelData.initUniformBuffers(&instance);

    // Scene Uniform Buffer:
    sceneData.setDevice(instance.getDevice());
    sceneData.initUniformBuffers(&instance);

    // Descriptor Pool + Sets:
    obj.initDescriptorPool();
    obj.initDescriptorSets(&modelData, &textureData, &sceneData);

    // CommandBuffers:
    drawer.setDevice(instance.getDevice());
    drawer.initCommandBuffers(instance.getCommandPool());

    // SyncObjects:
    drawer.initSyncObjects();
}

static void checkVkResult(VkResult err) {
    if (err == 0)
        return;

    if (err < 0)
        abort();
}

void initImGui() {
    // ~~~Following:
    // https://vkguide.dev/docs/extra-chapter/implementing_imgui/
    // https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
    VkDescriptorPoolSize poolSizes[] =
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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo info;
    info.Instance = instance.getInstance();
    info.PhysicalDevice = instance.getPhysicalDevice();
    info.Device = instance.getDevice();
    // info.QueueFamily = 
    info.Queue = instance.getGraphicsQueue();
    info.PipelineCache = VK_NULL_HANDLE;
    info.DescriptorPool = imguiPool;
    info.Subpass = 0;
    info.MinImageCount = 2;
    info.ImageCount = MAX_FRAMES_IN_FLIGHT;
    info.MSAASamples = VK_SAMPLE_COUNT_1_BIT; // find conversion between c and cpp impl
    info.Allocator = nullptr;
    info.CheckVkResultFn = checkVkResult;
    ImGui_ImplVulkan_Init(&info, renderer.getRenderPass());

    vk::CommandBuffer commandBuffer = instance.beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    instance.endSingleTimeCommands(commandBuffer);

    instance.getDevice().waitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

    void initScene() {
        // Scene setup:
        mainScene.setSceneData(&sceneData);
        mainScene.setCam(&cam);

        demoObj.setVulkanObject(&obj);
        demoObj.setModelData(&modelData);
        demoObj.setTextureData(&textureData);
        mainScene.addObject(&demoObj);
    }

    void mainLoop() {
        inputTimer = timer();

        while (!glfwWindowShouldClose(window)) {
            inputTimer.updateTime();

            glfwPollEvents();
            input.processKeyboardInput(window);
            mainScene.getCam()->processKeyboard(UP, inputTimer.getDeltaTime());
            //processInput(window, inputTimer.getDeltaTime());

            /*
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
            ImGui::Render();
            ImGui::EndFrame();*/

            drawer.drawFrame(
                &surface,
                &instance,
                &renderer,
                &mainScene
            );
        }

        instance.getDevice().waitIdle();
    }

    void cleanup() {
        // ImGui_ImplVulkan_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();

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