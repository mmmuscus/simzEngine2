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
}

vk::DescriptorPool imGuiInstance::initDescriptorPool() {
    VkDescriptorPoolSize poolSizes[] =
    {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT }
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
    poolInfo.poolSizeCount = std::size(poolSizes);
    poolInfo.pPoolSizes = poolSizes;

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
    vk::AttachmentDescription attachment = {};
    attachment.format = format;
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
        renderPass = device.createRenderPass(renderPassInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create render pass for imgui!");
    }
}

void imGuiInstance::initFramebuffers(vulkanSurface* surface) {
    size_t imageViewsSize = surface->getImageViews().size();

    framebuffers.resize(imageViewsSize);

    for (size_t i = 0; i < imageViewsSize; i++) {
        VkImageView attachment[1] = { surface->getImageViews()[i] };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachment;
        framebufferInfo.width = surface->getExtent().width;
        framebufferInfo.height = surface->getExtent().height;
        framebufferInfo.layers = 1;

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
