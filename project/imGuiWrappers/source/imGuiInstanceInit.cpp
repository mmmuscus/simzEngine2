#include "../include/imGuiInstance.h"

imGuiInstance::~imGuiInstance() {
    destroy();
}

void imGuiInstance::destroy() {
    if (!isCreated) return;

    device.waitIdle();
    device.destroyRenderPass(renderPass);
    renderPass = VK_NULL_HANDLE;

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    device.destroyDescriptorPool(descriptorPool);
    descriptorPool = VK_NULL_HANDLE;

    isCreated = false;
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

    auto info = vk::RenderPassCreateInfo(
        vk::RenderPassCreateFlags(),
        1, &attachment,
        1, &subpass,
        1, &dependency
    );

    try {
        renderPass = device.createRenderPass(info);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create ImGui renderpass");
    }
}

void imGuiInstance::init(GLFWwindow* _window, vulkanInstance* _instance, vulkanSurface* _surface) {
    isCreated = false;
    
    device = _instance->getDevice();

    // Vulkan inits
    initDescriptorPool();
    initRenderPass(_surface->getFormat());

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
    ImGui_ImplVulkan_InitInfo info = {};
    info.Instance = _instance->getInstance();
    info.PhysicalDevice = _instance->getPhysicalDevice();
    info.Device = device;
    // Dosent seem to be needed, but we are doing it for completeness sake
    info.QueueFamily = _instance->getGraphicsQueueFamily();
    info.Queue = _instance->getGraphicsQueue();
    info.PipelineCache = VK_NULL_HANDLE;
    info.DescriptorPool = descriptorPool;
    info.Subpass = 0;
    info.Allocator = nullptr;
    info.MinImageCount = IMGUI_MIN_IMAGE_COUNT;
    info.ImageCount = MAX_FRAMES_IN_FLIGHT;
    info.CheckVkResultFn = [](VkResult err) {
        if (err == 0) return;
        if (err < 0) abort();
    };
    ImGui_ImplVulkan_Init(&info, renderPass);

    vk::CommandBuffer commandBuffer = _instance->beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    _instance->endSingleTimeCommands(commandBuffer);

    _instance->getDevice().waitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    isCreated = true;
}