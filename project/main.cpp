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
#include "resourceManager/include/meshDataManager.h"
#include "resourceManager/include/textureDataManager.h"

#include "vulkanWrappers/include/vulkanInstance.h"
#include "vulkanWrappers/include/vulkanSurface.h"
#include "vulkanWrappers/include/vulkanObject.h"
#include "vulkanWrappers/include/vulkanRenderer.h"
#include "vulkanWrappers/include/vulkanDrawer.h"
#include "vulkanWrappers/include/vulkanMeshData.h"
#include "vulkanWrappers/include/vulkanTextureData.h"
#include "vulkanWrappers/include/vulkanModelData.h"
#include "vulkanWrappers/include/vulkanSceneData.h"

#include "imGuiWrappers/include/imGuiInstance.h"

#include "renderLogic/include/scene.h"
#include "renderLogic/include/object.h"
#include "renderLogic/include/camera.h"

class Application {
public:
    void run() {
        windowMngr.setSurface(&surface);
        windowMngr.initWindow();
        windowMngr.initGlfwInputHandling();
        initVulkan();
        imGuiInst.init(windowMngr.getWindow(), &instance, &surface);
        initScene();
        mainLoop();
        cleanup();
    }

private:
    // Input + window managing
    windowManager windowMngr;
    inputManager input;
    timer inputTimer;
    meshDataManager meshMngr;
    textureDataManager textureMngr;

    // Vulkan variables:
    vulkanInstance instance;
    vulkanSurface surface;
    vulkanObject diffuseObject;
    vulkanObject negativeObject;
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

        // DIFFUSE OBJECT:
        // Descriptor sets:
        diffuseObject.setDevice(instance.getDevice());
        diffuseObject.initDescriptors();

        // Pipeline:
        diffuseObject.initPipeline(
            surface.getExtent(), renderer.getRenderPass(), renderer.getMsaaSamples(),
            "shaders/vertexShaders/diffuseVert.spv", "shaders/fragmentShaders/diffuseFrag.spv"
        );

        // NEGATIVE OBJECT:
        // Descriptor sets:
        negativeObject.setDevice(instance.getDevice());
        negativeObject.initDescriptors();
        
        // Pipeline
        negativeObject.initPipeline(
            surface.getExtent(), renderer.getRenderPass(), renderer.getMsaaSamples(),
            "shaders/vertexShaders/diffuseVert.spv", "shaders/fragmentShaders/negativeFrag.spv"
        );

        // Color resources:
        renderer.initColorResources(&instance, surface.getFormat(), surface.getExtent());

        // Framebuffers:
        renderer.initFrameBuffers(surface.getImageViews(), surface.getExtent());

        // CommandPool:
        instance.initCommandPool();

        // CommandBuffers:
        drawer.setDevice(instance.getDevice());
        drawer.initCommandBuffers(instance.getCommandPool());

        // SyncObjects:
        drawer.initSyncObjects();

        // HELPERS FOR MESHES, TEXTURES:
        // Texture Sampler:
        textureSampler.init(instance.getDevice(), instance.getPhysicalDevice());

        // Dynamic Uniform Buffer:
        modelsBuffer.init(&instance);
    }

    void initScene() {
        // Scene setup:
        mainScene.init(
            &instance, 
            diffuseObject.getSceneDescriptorSetLayout(),
            diffuseObject.getModelDescriptorPool()
        );

        // Add objects
        mainScene.addObject(new object(
            &instance, &diffuseObject,
            &meshMngr, "models/viking_room.objj", &modelsBuffer,
            &textureMngr, "textures/viking_room.png", &textureSampler
        ));
        mainScene.addObject(new object(
            &instance, &negativeObject,
            &meshMngr, "models/tank.objj", &modelsBuffer,
            &textureMngr, "textures/camouflage.jpg", &textureSampler,
            glm::vec3(0.0f, -1.75f, -0.75f),
            glm::vec3(0.0f, 0.0f, 3.14f)
        ));

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
                // ImGui framebuffers recreation:
                imGuiInst.recreateFramebuffers(&surface);
                // Admin stuff
                surface.setShouldRecreateSwapChain(false);
                drawer.resetImageIndex();
            }

            // Acquire the next swapchain image
            drawer.getNextImage(&surface);

            // Update translation of scene (and its objects)
            mainScene.updateScene(drawer.getCurrentFrame(), surface.getExtent());

            // Record and submit engine command buffer
            drawer.drawFrame(
                &surface,
                &renderer,
                &mainScene,
                instance.getGraphicsQueue()
            );

            // record and submit ImGui commandBuffer
            if (imGuiInst.getIsEnabled()) {
                imGuiInst.presentGui(
                    surface.getShouldRecreateSwapChain(), &mainScene,
                    &instance, &diffuseObject,
                    &meshMngr, &textureMngr,
                    &modelsBuffer, &textureSampler
                );
                imGuiInst.drawFrame(&surface, &instance, drawer.getImageIndex());
            }

            // Present the frame
            drawer.presentFrame(&surface, instance.getPresentQueue());
        }

        instance.getDevice().waitIdle();
    }

    void cleanup() {

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