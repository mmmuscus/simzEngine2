#include "../include/vulkanRenderPass.h"

vulkanRenderPass::~vulkanRenderPass() {
    for (auto framebuffer : framebuffers) {
        device.destroyFramebuffer(framebuffer);
    }

    device.destroyRenderPass(renderPass);
}

void vulkanRenderPass::initRenderPass(vk::Format imageFormat) {
    auto colorAttachment = vk::AttachmentDescription(
        vk::AttachmentDescriptionFlags(),
        imageFormat,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    );

    auto colorAttachmentRef = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);

    auto subpass = vk::SubpassDescription(
        vk::SubpassDescriptionFlags(),
        vk::PipelineBindPoint::eGraphics,
        0, nullptr,                             // input attachment
        1, &colorAttachmentRef                  // color attachment
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
        1, &colorAttachment,
        1, &subpass,
        1, &dependency
    );

    try {
        renderPass = device.createRenderPass(renderPassInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void vulkanRenderPass::initFrameBuffers(std::vector<vk::ImageView> imageViews, vk::Extent2D extent) {
    framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        vk::ImageView attachments[] = {
            imageViews[i]
        };

        auto framebufferInfo = vk::FramebufferCreateInfo(
            vk::FramebufferCreateFlags(),
            renderPass,
            1, attachments,
            extent.width, extent.height,
            1
        );

        try {
            framebuffers[i] = device.createFramebuffer(framebufferInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}