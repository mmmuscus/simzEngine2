#include "../include/imGuiInstance.h"

imGuiInstance::~imGuiInstance() {
    destroy();
}

void imGuiInstance::destroy() {
    if (!isCreated) return;

    device.waitIdle();
    destroyFramebuffers();
    device.destroyRenderPass(renderPass);
    renderPass = VK_NULL_HANDLE;

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    device.destroyDescriptorPool(descriptorPool);
    descriptorPool = VK_NULL_HANDLE;
    device.freeCommandBuffers(commandPool, commandBuffers);
    device.destroyCommandPool(commandPool);
    commandPool = VK_NULL_HANDLE;

    isCreated = false;
}

void imGuiInstance::destroyFramebuffers() {
    for (size_t i = 0; i < framebuffers.size(); i++) {
        device.destroyFramebuffer(framebuffers[i]);
        framebuffers[i] = VK_NULL_HANDLE;
    }
}

void imGuiInstance::initDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eCombinedImageSampler,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        )
    };

    auto poolInfo = vk::DescriptorPoolCreateInfo(
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
        static_cast<uint32_t>(poolSizes.size()), poolSizes.data()
    );

    try {
        descriptorPool = device.createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("Failed to create ImGui descriptor pool");
    }
}

void imGuiInstance::initRenderPass(vk::Format _format) {
    auto attachment = vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        _format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eLoad,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR
    );

    auto colorAttachment = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    
    auto subpass = vk::SubpassDescription(
        vk::SubpassDescriptionFlags(),
        vk::PipelineBindPoint::eGraphics,
        0, nullptr,
        1, &colorAttachment
    );

    auto dependency = vk::SubpassDependency(
        static_cast<uint32_t>(VK_SUBPASS_EXTERNAL),
        static_cast<uint32_t>(0),
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        // This might need to be changed to vk::AccessFlagBits::eColorAttachmentWrite
        vk::AccessFlagBits(0),
        vk::AccessFlagBits::eColorAttachmentWrite
    );

    auto renderPassCreateInfo = vk::RenderPassCreateInfo(
        vk::RenderPassCreateFlags(),
        1, &attachment,
        1, &subpass,
        1, &dependency
    );

    try {
        renderPass = device.createRenderPass(renderPassCreateInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create ImGui renderpass");
    }
}

void imGuiInstance::initFramebuffers(vulkanSurface* _surface) {
    framebuffers.resize(_surface->getImageViews().size());

    for (size_t i = 0; i < _surface->getImageViews().size(); i++) {
        std::array<vk::ImageView, 1> attachment = { _surface->getImageViews()[i] };

        auto framebufferCreateInfo = vk::FramebufferCreateInfo(
            vk::FramebufferCreateFlags(),
            renderPass,
            static_cast<uint32_t>(attachment.size()), attachment.data(),
            _surface->getExtent().width, _surface->getExtent().height,
            1
        );

        try {
            framebuffers[i] = device.createFramebuffer(framebufferCreateInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create ImGui framebuffer");
        }
    }
}

void imGuiInstance::initCommandPool(QueueFamilyIndices queueFamilyIndices) {
    vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        queueFamilyIndices.graphicsFamily.value()
    };
    
    try {
        commandPool = device.createCommandPool(commandPoolCreateInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("Failed to create imGui command pool!");
    }
}

void imGuiInstance::initCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {
        commandPool,
        vk::CommandBufferLevel::ePrimary,
        static_cast<uint32_t>(commandBuffers.size())
    };

    try {
        commandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("Failed to create imGui command buffers!");
    }
}

void imGuiInstance::initEditor(vulkanObjectManager* _objectManager,
    meshDataManager* _meshManager, textureDataManager* _textureManager
) {
    objectManager = _objectManager;
    meshManager = _meshManager;
    textureManager = _textureManager;
}

void imGuiInstance::init(GLFWwindow* _window, vulkanInstance* _instance, vulkanSurface* _surface,
    vulkanObjectManager* _objectManager, meshDataManager* _meshManager, textureDataManager* _textureManager
) {
    isCreated = false;
    
    device = _instance->getDevice();

    // Vulkan inits
    initDescriptorPool();
    initRenderPass(_surface->getFormat());
    initFramebuffers(_surface);
    initCommandPool(_instance->findQueueFamilies(_instance->getPhysicalDevice()));
    initCommandBuffers();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Render bindings
    ImGui_ImplGlfw_InitForVulkan(_window, true);
    ImGui_ImplVulkan_InitInfo imGuiInitInfo = {};
    imGuiInitInfo.Instance = _instance->getInstance();
    imGuiInitInfo.PhysicalDevice = _instance->getPhysicalDevice();
    imGuiInitInfo.Device = device;
    // Dosent seem to be needed, but we are doing it for completeness sake
    imGuiInitInfo.QueueFamily = _instance->getGraphicsQueueFamily();
    imGuiInitInfo.Queue = _instance->getGraphicsQueue();
    imGuiInitInfo.PipelineCache = VK_NULL_HANDLE;
    imGuiInitInfo.DescriptorPool = descriptorPool;
    imGuiInitInfo.Subpass = 0;
    imGuiInitInfo.Allocator = nullptr;
    imGuiInitInfo.MinImageCount = IMGUI_MIN_IMAGE_COUNT;
    imGuiInitInfo.ImageCount = MAX_FRAMES_IN_FLIGHT;
    imGuiInitInfo.CheckVkResultFn = [](VkResult err) {
        if (err == 0) return;
        if (err < 0) abort();
    };
    ImGui_ImplVulkan_Init(&imGuiInitInfo, renderPass);

    // Upload fonts
    vk::CommandBuffer commandBuffer = _instance->beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    _instance->endSingleTimeCommands(commandBuffer);

    _instance->getDevice().waitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    // Add references to managers for editor:
    initEditor(_objectManager, _meshManager, _textureManager);

    isCreated = true;
}