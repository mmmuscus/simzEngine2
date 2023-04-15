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

const int IMGUI_MIN_IMAGE_COUNT = 2;

class Application {
public:
    void run() {
        wndwManager.setSurface(&surface);
        wndwManager.initWindow();
        wndwManager.initGlfwInputHandling();
        initVulkan();
        initImGui();
        initScene();
        mainLoop();
        cleanup();
    }

private:
    // Input + window managing
    windowManager wndwManager;
    inputManager input;
    timer inputTimer;

    // Vulkan variables:
    vulkanInstance instance;
    vulkanSurface surface;
    vulkanObject obj;
    vulkanRenderer renderer;
    vulkanDrawer drawer;
    vulkanModelData modelData;
    vulkanTextureData textureData;
    vulkanSceneData sceneData;

    // Scene variables:
    scene mainScene;
    object demoObj;
    camera cam = camera(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // ImGui variables PLS REMOVE
    vk::RenderPass imGuiRenderPass;
    std::vector<vk::Framebuffer> imGuiFramebuffers;

    void initVulkan() {
        instance.listExtensions();

         // Instance:
        instance.initInstance();
        instance.initCallback();

        // Surface:
        surface.setWindow(wndwManager.getWindow());
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
        // https://frguthmann.github.io/posts/vulkan_imgui/
        
        // DESCRIPTOR POOL FOR IMGUI:
        VkDescriptorPoolSize poolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT }
        };

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        poolInfo.poolSizeCount = std::size(poolSizes);
        poolInfo.pPoolSizes = poolSizes;

        vk::DescriptorPool imguiPool;
        try {
            imguiPool = instance.getDevice().createDescriptorPool(poolInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create ImGui descriptor pool!");
        }
    
        // RENDER PASS FOR IMGUI:
        vk::AttachmentDescription attachment = {};
        attachment.format = surface.getFormat();
        attachment.samples = vk::SampleCountFlagBits::e1;
        attachment.loadOp = vk::AttachmentLoadOp::eLoad;
        attachment.storeOp = vk::AttachmentStoreOp::eStore;
        attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
        attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference color_attachment = {};
        color_attachment.attachment = 0;
        color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass = {};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment;

        vk::SubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.srcAccessMask = {};
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

        vk::RenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &attachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        try {
            imGuiRenderPass = instance.getDevice().createRenderPass(renderPassInfo);
        } 
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create render pass for imgui!");
        }

        // FRAMEBUFFERS FOR IMGUI
        imGuiFramebuffers.resize(surface.getImageViews().size());

        for (size_t i = 0; i < surface.getImageViews().size(); i++) {
            VkImageView attachment[1] = { surface.getImageViews()[i] };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = imGuiRenderPass;
            framebufferInfo.pAttachments = attachment;
            framebufferInfo.width = surface.getExtent().width;
            framebufferInfo.height = surface.getExtent().height;
            framebufferInfo.layers = 1;

            try {
                imGuiFramebuffers[i] = instance.getDevice().createFramebuffer(framebufferInfo);
            }
            catch (vk::SystemError err) {
                throw std::runtime_error("failed to create imgui framebuffer!");
            }
        }

        // imgui init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(wndwManager.getWindow(), true);
        ImGui_ImplVulkan_InitInfo info;
        info.Instance = instance.getInstance();
        info.PhysicalDevice = instance.getPhysicalDevice();
        info.Device = instance.getDevice();
        // info.QueueFamily = 
        info.Queue = instance.getGraphicsQueue();
        info.PipelineCache = VK_NULL_HANDLE;
        info.DescriptorPool = imguiPool;
        info.Subpass = 0;
        info.MinImageCount = IMGUI_MIN_IMAGE_COUNT;
        info.ImageCount = MAX_FRAMES_IN_FLIGHT;
        info.MSAASamples = VK_SAMPLE_COUNT_1_BIT; // find conversion between c and cpp impl
        info.Allocator = nullptr;
        info.CheckVkResultFn = checkVkResult;
        ImGui_ImplVulkan_Init(&info, imGuiRenderPass);

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

        // Add object
        demoObj = object(&obj, &modelData, &textureData);
        mainScene.addObject(&demoObj);
    }

    void mainLoop() {
        inputTimer = timer();
        mainScene.resetSceneTimer();

        while (!glfwWindowShouldClose(wndwManager.getWindow())) {
            inputTimer.updateTime();

            // Process inputs and update camera
            glfwPollEvents();
            input.processKeyboardInput(wndwManager.getWindow());
            wndwManager.checkIfWindowShouldClose();
            mainScene.getCam()->processKeyboard(inputTimer.getDeltaTime());
            mainScene.getCam()->processMouseMovement();
            input.resetOffset();

            // Recreate Swapchin if necessary
            if (surface.getShouldRecreateSwapChain()) {
                std::cout << "swap chain out of date/suboptimal/window resized - recreating" << std::endl;
                surface.recreateSwapChain(&renderer, &instance);
                surface.setShouldRecreateSwapChain(false);
                drawer.resetImageIndex();
            }

            // Acquire the next swapchain image
            drawer.getNextImage(&surface);

            // Update translation of scene (and its objects)
            mainScene.updateScene(drawer.getCurrentFrame(), surface.getExtent());

            // rendering ImGui + engine
            /*ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
            ImGui::Render();*/

            drawer.drawFrame(
                &surface,
                &instance,
                &renderer,
                &mainScene
            );

            /*uint32_t imageIndex = drawer.getImgIdx();

            vk::CommandBuffer commandBuffer = instance.beginSingleTimeCommands();

            std::array<vk::ClearValue, 1> clearValues;
            clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

            auto renderPassInfo = vk::RenderPassBeginInfo(
                imGuiRenderPass, imGuiFramebuffers[imageIndex],
                vk::Rect2D(vk::Offset2D(0, 0), surface.getExtent()),
                static_cast<uint32_t>(clearValues.size()), clearValues.data()
            );

            commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
            commandBuffer.endRenderPass();
            instance.endSingleTimeCommands(commandBuffer);

            vk::SwapchainKHR swapChains[] = { surface.getSwapChain() };

            auto presentInfo = vk::PresentInfoKHR(
                0, nullptr,
                1, swapChains,
                &imageIndex
            );

            vk::Result resultPresent;
            resultPresent = instance.getPresentQueue().presentKHR(presentInfo);

            ImGui::EndFrame();*/
        }

        instance.getDevice().waitIdle();
    }

    void cleanup() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
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