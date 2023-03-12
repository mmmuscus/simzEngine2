#include "../include/vulkanRenderer.h"

vulkanRenderer::~vulkanRenderer() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(renderFinishedSemaphores[i]);
        device.destroySemaphore(imageAvailableSemaphores[i]);
        device.destroyFence(inFlightFences[i]);
    }

	device.destroyCommandPool(commandPool);
}

void vulkanRenderer::initCommandPool(vulkanInstance* instance) {
    QueueFamilyIndices queueFamilyIndices = 
        instance->findQueueFamilies(instance->getPhysicalDevice());

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    try {
        commandPool = device.createCommandPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void vulkanRenderer::initCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    try {
        commandBuffers = device.allocateCommandBuffers(allocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void vulkanRenderer::initSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    try {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            imageAvailableSemaphores[i] = device.createSemaphore({});
            renderFinishedSemaphores[i] = device.createSemaphore({});
            inFlightFences[i] = device.createFence({ vk::FenceCreateFlagBits::eSignaled });
        }
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void vulkanRenderer::recordCommandBuffer(
    vk::CommandBuffer commandBuffer,
    vk::Pipeline graphicsPipeline,
    vulkanRenderPass* renderPass,
    vk::Extent2D extent,
    uint32_t imageIndex
) {
    vk::CommandBufferBeginInfo beginInfo = {};

    try {
        commandBuffer.begin(beginInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    vk::RenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.renderPass = renderPass->getRenderPass();
    renderPassInfo.framebuffer = renderPass->getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = vk::Offset2D(0, 0);
    renderPassInfo.renderArea.extent = extent;

    vk::ClearValue clearColor = { std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f } };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    commandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D(0, 0);
    scissor.extent = extent;
    commandBuffer.setScissor(0, 1, &scissor);

    commandBuffer.draw(3, 1, 0, 0);
    commandBuffer.endRenderPass();

    try {
        commandBuffer.end();
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void vulkanRenderer::drawFrame(
    vulkanSurface* surface,
    vulkanInstance* instance,
    vk::Pipeline graphicsPipeline,
    vulkanRenderPass* renderPass
) {
    device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    uint32_t imageIndex;
    try {
        vk::ResultValue result = device.acquireNextImageKHR(
            surface->getSwapChain(),
            std::numeric_limits<uint64_t>::max(),
            imageAvailableSemaphores[currentFrame],
            nullptr);
        imageIndex = result.value;
    } catch (vk::OutOfDateKHRError err) {
        surface->recreateSwapChain(
            renderPass,
            instance);
        return;
    } catch (vk::SystemError err) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    device.resetFences(1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(
        commandBuffers[currentFrame],
        graphicsPipeline,
        renderPass,
        surface->getExtent(),
        imageIndex);

    vk::SubmitInfo submitInfo = {};

    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    try {
        instance->getGraphicsQueue().submit(submitInfo, inFlightFences[currentFrame]);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = { surface->getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vk::Result resultPresent;
    try {
        resultPresent = instance->getPresentQueue().presentKHR(presentInfo);
    }
    catch (vk::OutOfDateKHRError err) {
        resultPresent = vk::Result::eErrorOutOfDateKHR;
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eSuboptimalKHR || framebufferResized) {
        std::cout << "swap chain out of date/suboptimal/window resized - recreating" << std::endl;
        framebufferResized = false;
        surface->recreateSwapChain(
            renderPass,
            instance);
        return;
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}