#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "engine/general/include/generalIncludes.h"
#include "engine/general/include/timer.h"

#include "engine/resourceManager/include/windowManager.h"
#include "engine/resourceManager/include/inputManager.h"
#include "engine/resourceManager/include/meshDataManager.h"
#include "engine/resourceManager/include/textureDataManager.h"
#include "engine/resourceManager/include/vulkanPipelineManager.h"

#include "engine/vulkanWrappers/include/vulkanInstance.h"
#include "engine/vulkanWrappers/include/vulkanSurface.h"
#include "engine/vulkanWrappers/include/vulkanPipeline.h"
#include "engine/vulkanWrappers/include/vulkanRenderer.h"
#include "engine/vulkanWrappers/include/vulkanDrawer.h"
#include "engine/vulkanWrappers/include/vulkanMeshData.h"
#include "engine/vulkanWrappers/include/vulkanTextureData.h"
#include "engine/vulkanWrappers/include/vulkanModelData.h"
#include "engine/vulkanWrappers/include/vulkanSceneData.h"

#include "engine/imGuiWrappers/include/imGuiInstance.h"

#include "engine/sceneSystem/include/scene.h"
#include "engine/sceneSystem/include/gameObject.h"
#include "engine/sceneSystem/include/camera.h"

class Application {
public:
    void run() {
        windowMngr.setSurface(&surface);
        windowMngr.initWindow();
        windowMngr.initGlfwInputHandling();
        initVulkan();
        if (enableImGuiDebugger)
            imGuiInst.init(windowMngr.getWindow(), &instance, &surface,
                &vulkanPipelineMngr, &meshMngr, &textureMngr);
        initScene();

        mainLoop();

        mainScene.destroy();
        if (enableImGuiDebugger)
            imGuiInst.destroy();
        destroyVulkan();
    }

private:
    // Input + window managing
    windowManager windowMngr;
    inputManager input;
    timer inputTimer;

    meshDataManager meshMngr;
    textureDataManager textureMngr;
    vulkanPipelineManager vulkanPipelineMngr;

    // Vulkan variables:
    vulkanInstance instance;
    vulkanSurface surface;
    vulkanPipeline* diffusePipeline;
    vulkanPipeline* negativePipeline;
    vulkanRenderer renderer;
    vulkanDrawer drawer;
    vulkanDynamicUniformBuffer modelsBuffer;
    vulkanTextureSampler textureSampler;

    // Scene variables:
    scene mainScene;

    // ImGui variables:
    imGuiInstance imGuiInst;

    void initVulkan() {
        // Instance:
        instance.listExtensions();
        instance.initInstance();
        instance.initCallback();

        // Surface:
        surface.setWindow(windowMngr.getWindow());
        surface.setInstance(instance.getInstancePtr());
        surface.initSurface();

        // Devices:
        instance.setSurface(surface.getSurface());
        instance.initPhysicalDevice();
        instance.initDevice();

        // SwapChain:
        surface.setDevice(instance.getDevicePtr());
        surface.initSwapChain(&instance);
        surface.initImageViews(&instance);

        // Msaa Samples:
        renderer.initMsaaSamples(instance.getPhysicalDevice());

        // Depth resources:
        renderer.initDepthResources(&instance, surface.getExtent());

        // Render Pass:
        renderer.setDevice(instance.getDevicePtr());
        renderer.initRenderPass(surface.getFormat());

        // DIFFUSE OBJECT:
        diffusePipeline = vulkanPipelineMngr.addVulkanPipeline(
            "DIFFUSE", instance.getDevicePtr(),
            surface.getExtent(), renderer.getRenderPass(), renderer.getMsaaSamples(),
            "../../../assets/shaders/vertexShaders/diffuseVert.spv", "../../../assets/shaders/fragmentShaders/diffuseFrag.spv"
        );

        // NEGATIVE OBJECT:
        negativePipeline = vulkanPipelineMngr.addVulkanPipeline(
            "NEGATIVE", instance.getDevicePtr(),
            surface.getExtent(), renderer.getRenderPass(), renderer.getMsaaSamples(),
            "../../../assets/shaders/vertexShaders/diffuseVert.spv", "../../../assets/shaders/fragmentShaders/negativeFrag.spv"
        );

        // Color Resources:
        renderer.initColorResources(&instance, surface.getFormat(), surface.getExtent());

        // Framebuffers:
        renderer.initFrameBuffers(surface.getImageViews(), surface.getExtent());

        // CommandPool:
        instance.initCommandPool();

        // CommandBuffers:
        drawer.setDevice(instance.getDevicePtr());
        drawer.initCommandBuffers(instance.getCommandPool());

        // SyncObjects:
        drawer.initSyncObjects();

        // HELPERS FOR MESHES, TEXTURES:
        // Texture Sampler:
        textureSampler.init(instance.getDevicePtr(), instance.getPhysicalDevice());

        // Dynamic Uniform Buffer:
        modelsBuffer.init(&instance);
    }

    void destroyVulkan() {
        instance.getDevicePtr()->waitIdle();

        // Destroying meshes and textures:
        meshMngr.destroyList();
        textureMngr.destroyList();

        // Dynamic Uniform Buffer:
        modelsBuffer.destroy();

        // Texture Sampler:
        textureSampler.destroy();

        // Sync Objects:
        drawer.destroySyncObjects();

        // Command Buffers are only detroyed when the Command Pool is destroyed
        // Command Pool:
        instance.destroyCommandPool();

        // Framebuffers + Color Resources:
        renderer.destroyFramebuffers();
        renderer.destroyColorResources();

        // NEGATIVE OBJECT + DIFFUSE OBJECT:
        vulkanPipelineMngr.destroyList();

        // Render Pass + Depth Resources:
        // Msaa Samples is not a vulkan object
        renderer.destroyRenderPass();
        renderer.destroyDepthResources();

        // SwapChain + ImageViews:
        surface.destroyImageViews();
        surface.destroySwapChain();

        // Device + PhysicalDevice dosent need to be destroyed explicitly

        // Surface:
        surface.destroySurface();

        // Callback:
        // Instance dosent need to be destroyed explicily
        instance.destroyCallback();
    }

    void initScene() {
        // Scene setup:
        mainScene.init(
            &instance,
            diffusePipeline->getSceneDescriptorSetLayout(),
            diffusePipeline->getSceneDescriptorPool()
        );

        // Add objects
        mainScene.addObject(new gameObject(
            &instance, diffusePipeline,
            &meshMngr, "../../../assets/models/viking_room.objj", &modelsBuffer,
            &textureMngr, "../../../assets/textures/viking_room.png", &textureSampler,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        ));
        /*mainScene.addObject(new gameObject(
            &instance, negativePipeline,
            &meshMngr, "assets/models/tank.objj", &modelsBuffer,
            &textureMngr, "assets/textures/camouflage.jpg", &textureSampler,
            glm::vec3(0.0f, -2.25f, -0.75f),
            glm::vec3(0.0f, 0.0f, 3.14f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        ));*/

        mainScene.defragmentObjectNumbers();
    }

    void mainLoop() {
        inputTimer = timer();
        mainScene.resetSceneTimer();

        while (!glfwWindowShouldClose(windowMngr.getWindow())) {
            inputTimer.updateTime();
            mainScene.defragmentObjectNumbers();

            // Process inputs and update camera
            glfwPollEvents();
            input.processKeyboardInput(windowMngr.getWindow());
            windowMngr.checkIfWindowShouldClose();
            mainScene.getCam()->processKeyboard(inputTimer.getDeltaTime());
            mainScene.getCam()->processMouseMovement();
            input.resetOffset();

            // Recreate Swapchain if necessary
            if (surface.getShouldRecreateSwapChain()) {
                std::cout << "swap chain out of date/suboptimal/window resized - recreating" << std::endl;
                surface.recreateSwapChain(&renderer, &instance);
                // ImGui should be told IF the min image count changed (shouldnt apply to us)
                if (enableImGuiDebugger)
                    imGuiInst.recreateFramebuffers(&surface);
                // Admin stuff
                surface.setShouldRecreateSwapChain(false);
                drawer.resetImageIndex();
            }

            // Acquire the next swapchain image
            drawer.getNextImage(&surface);

            // This is where the physics system updates used to be

            mainScene.updateScene(drawer.getCurrentFrame(), surface.getExtent());

            // Record and submit engine command buffer
            drawer.drawFrame(
                &surface,
                &renderer,
                &mainScene
            );

            if (enableImGuiDebugger) {
                if (!surface.getShouldRecreateSwapChain())
                    imGuiInst.drawGui(&mainScene, &instance, diffusePipeline,
                        &modelsBuffer, &textureSampler);

                imGuiInst.drawFrame(&surface, drawer.getImageIndex(), drawer.getCurrentFrame());
            }

            drawer.submitCommandBuffer(surface.getShouldRecreateSwapChain(), instance.getGraphicsQueue());

            // Present the frame
            drawer.presentFrame(&surface, instance.getPresentQueue());
        }

        instance.getDevicePtr()->waitIdle();
    }
};

int main() {
    Application app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        // Errors can happen that make it seem like Vulkan is not working but its just
        // not getting destructed
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}