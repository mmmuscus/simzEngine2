#include "../include/imGuiInstance.h"

imGuiInstance::~imGuiInstance() {
    if (!isInstance)
        return;

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    destroyFramebuffers();

    device.destroyRenderPass(renderPass);
}

void imGuiInstance::init(
    GLFWwindow* window,
    vulkanInstance* instance,
    vulkanSurface* surface
) {
    // https://frguthmann.github.io/posts/vulkan_imgui/
    device = instance->getDevice();
    initRenderPass(surface->getFormat());
    initFramebuffers(surface);
    initImGui(window, instance);
    isEnabled = true;
}

vk::DescriptorPool imGuiInstance::initDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eCombinedImageSampler,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        )
    };

    auto poolInfo = vk::DescriptorPoolCreateInfo(
        vk::DescriptorPoolCreateFlags(),
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
        static_cast<uint32_t>(poolSizes.size()), poolSizes.data()
    );

    vk::DescriptorPool imGuiPool;
    try {
        imGuiPool = device.createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("failed to create ImGui descriptor pool!");
    }

    return imGuiPool;
}

void imGuiInstance::initRenderPass(vk::Format format) {
    auto attachment = vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        format,
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
        static_cast<vk::AccessFlagBits>(0),
        vk::AccessFlagBits::eColorAttachmentWrite
    );

    auto renderPassInfo = vk::RenderPassCreateInfo(
        vk::RenderPassCreateFlags(),
        1, &attachment,
        1, &subpass,
        1, &dependency
    );

    try {
        renderPass = device.createRenderPass(renderPassInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create render pass for imgui!");
    }
}

void imGuiInstance::initFramebuffers(vulkanSurface* surface) {
    framebuffers.resize(surface->getImageViews().size());

    for (size_t i = 0; i < surface->getImageViews().size(); i++) {
        std::array<vk::ImageView, 1> attachment = { surface->getImageViews()[i] };

        auto framebufferInfo = vk::FramebufferCreateInfo(
            vk::FramebufferCreateFlags(),
            renderPass,
            static_cast<uint32_t>(attachment.size()), attachment.data(),
            surface->getExtent().width, surface->getExtent().height,
            1
        );

        try {
            framebuffers[i] = device.createFramebuffer(framebufferInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create imgui framebuffer!");
        }
    }
}

void imGuiInstance::initImGui(GLFWwindow* window, vulkanInstance* instance) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo info;
    info.Instance = instance->getInstance();
    info.PhysicalDevice = instance->getPhysicalDevice();
    info.Device = instance->getDevice();
    info.QueueFamily = instance->getGraphicsQueueFamily();
    info.Queue = instance->getGraphicsQueue();
    info.PipelineCache = VK_NULL_HANDLE;
    info.DescriptorPool = initDescriptorPool();
    info.Subpass = 0;
    info.MinImageCount = IMGUI_MIN_IMAGE_COUNT;
    info.ImageCount = MAX_FRAMES_IN_FLIGHT;
    info.MSAASamples = VK_SAMPLE_COUNT_1_BIT; // find conversion between c and cpp impl
    info.Allocator = nullptr;
    info.CheckVkResultFn = checkVkResult;
    ImGui_ImplVulkan_Init(&info, renderPass);

    vk::CommandBuffer commandBuffer = instance->beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    instance->endSingleTimeCommands(commandBuffer);

    instance->getDevice().waitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void imGuiInstance::checkVkResult(VkResult err) {
    if (err == 0)
        return;

    if (err < 0)
        abort();
}
