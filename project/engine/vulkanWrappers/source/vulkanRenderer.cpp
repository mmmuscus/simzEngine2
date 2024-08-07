#include "../include/vulkanRenderer.h"

vulkanRenderer::~vulkanRenderer() {
    destroyFramebuffers();
    destroyColorResources();
    destroyDepthResources();
    destroyRenderPass();
}

void vulkanRenderer::destroyFramebuffers() {
    for (size_t i = 0; i < framebuffers.size(); i++) {
        device->destroyFramebuffer(framebuffers[i]);
        framebuffers[i] = VK_NULL_HANDLE;
    }
}

void vulkanRenderer::destroyColorResources() {
    device->destroyImageView(colorImageView);
    colorImageView = VK_NULL_HANDLE;
    device->destroyImage(colorImage);
    colorImage = VK_NULL_HANDLE;
    device->freeMemory(colorImageMemory);
    colorImageMemory = VK_NULL_HANDLE;
}

void vulkanRenderer::destroyDepthResources() {
    device->destroyImageView(depthImageView);
    depthImageView = VK_NULL_HANDLE;
    device->destroyImage(depthImage);
    depthImage = VK_NULL_HANDLE;
    device->freeMemory(depthImageMemory);
    depthImageMemory = VK_NULL_HANDLE;
}

void vulkanRenderer::destroyRenderPass() {
    device->destroyRenderPass(renderPass);
    renderPass = VK_NULL_HANDLE;
}

void vulkanRenderer::initRenderPass(vk::Format imageFormat) {
    auto colorAttachment = vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        imageFormat,
        msaaSamples,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal
    );

    auto depthAttachment = vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        depthFormat,
        msaaSamples,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
    );

    auto colorAttachmentResolve = vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        imageFormat,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    );

    auto colorAttachmentRef = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    auto depthAttachmentRef = vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    auto colorAttachmentResolveRef = vk::AttachmentReference(2, vk::ImageLayout::eColorAttachmentOptimal);

    auto subpass = vk::SubpassDescription(
        vk::SubpassDescriptionFlags(),
        vk::PipelineBindPoint::eGraphics,
        0, nullptr,                             // input attachment
        1, &colorAttachmentRef,                 // color attachment
        &colorAttachmentResolveRef,             // resolve attachment
        &depthAttachmentRef                     // depth attachment
    );

    auto dependency = vk::SubpassDependency(
        static_cast<uint32_t>(VK_SUBPASS_EXTERNAL),
        static_cast<uint32_t>(0),
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        static_cast<vk::AccessFlagBits>(0),
        vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
    );

    std::array<vk::AttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    auto renderPassInfo = vk::RenderPassCreateInfo(
        vk::RenderPassCreateFlags(),
        static_cast<uint32_t>(attachments.size()), attachments.data(),
        1, &subpass,
        1, &dependency
    );

    try {
        renderPass = device->createRenderPass(renderPassInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void vulkanRenderer::initFrameBuffers(std::vector<vk::ImageView> imageViews, vk::Extent2D extent) {
    framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        std::array<vk::ImageView, 3> attachments = { colorImageView, depthImageView, imageViews[i] };

        auto framebufferInfo = vk::FramebufferCreateInfo(
            vk::FramebufferCreateFlags(),
            renderPass,
            static_cast<uint32_t>(attachments.size()), attachments.data(),
            extent.width, extent.height,
            1
        );

        try {
            framebuffers[i] = device->createFramebuffer(framebufferInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void vulkanRenderer::initMsaaSamples(vk::PhysicalDevice physicalDevice) {
    msaaSamples = getMaxUsableSampleCount(physicalDevice);
}

void vulkanRenderer::initColorResources(vulkanInstance* instance, vk::Format colorFormat, vk::Extent2D extent) {
    instance->initImage(
        extent.width, extent.height, colorFormat, 1, msaaSamples, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageLayout::eUndefined,
        colorImage, colorImageMemory
    );
    colorImageView = instance->initImageView(colorImage, colorFormat, vk::ImageAspectFlagBits::eColor, 1);
}

void vulkanRenderer::initDepthResources(vulkanInstance* instance, vk::Extent2D extent) {
    depthFormat = findDepthFormat(instance);

    instance->initImage(
        extent.width, extent.height, depthFormat, 1, msaaSamples, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageLayout::eUndefined,
        depthImage, depthImageMemory
    );
    depthImageView = instance->initImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1);
}

vk::Format vulkanRenderer::findDepthFormat(vulkanInstance* instance) {
    return instance->findSupportedFormat(
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

vk::SampleCountFlagBits vulkanRenderer::getMaxUsableSampleCount(vk::PhysicalDevice physicalDevice) {
    vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();

    vk::SampleCountFlags counts =
        physicalDeviceProperties.limits.framebufferColorSampleCounts &
        physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    if (counts & vk::SampleCountFlagBits::e64) return vk::SampleCountFlagBits::e64;
    if (counts & vk::SampleCountFlagBits::e32) return vk::SampleCountFlagBits::e32;
    if (counts & vk::SampleCountFlagBits::e16) return vk::SampleCountFlagBits::e16;
    if (counts & vk::SampleCountFlagBits::e8) return vk::SampleCountFlagBits::e8;
    if (counts & vk::SampleCountFlagBits::e4) return vk::SampleCountFlagBits::e4;
    if (counts & vk::SampleCountFlagBits::e2) return vk::SampleCountFlagBits::e2;

    return vk::SampleCountFlagBits::e1;
}