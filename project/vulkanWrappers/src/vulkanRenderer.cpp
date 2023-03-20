#include "../include/vulkanRenderer.h"

vulkanRenderer::~vulkanRenderer() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(renderFinishedSemaphores[i]);
        device.destroySemaphore(imageAvailableSemaphores[i]);
        device.destroyFence(inFlightFences[i]);
    }
}

void vulkanRenderer::initCommandBuffers(vk::CommandPool commandPool) {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    auto allocInfo = vk::CommandBufferAllocateInfo(
        commandPool,
        vk::CommandBufferLevel::ePrimary,
        static_cast<uint32_t>(commandBuffers.size())
    );

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
    vulkanObject* object,
    vulkanRenderPass* renderPass,
    vk::Extent2D extent,
    uint32_t imageIndex
) {
    auto beginInfo = vk::CommandBufferBeginInfo();

    try {
        commandBuffer.begin(beginInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    auto renderArea = vk::Rect2D(vk::Offset2D(0, 0), extent);
    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);

    auto renderPassInfo = vk::RenderPassBeginInfo(
        renderPass->getRenderPass(),
        renderPass->getFramebuffers()[imageIndex],
        renderArea,
        static_cast<uint32_t>(clearValues.size()), clearValues.data()
    );

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, object->getPipeline());

    auto viewport = vk::Viewport(
        0.0f, 0.0f,                 // x, y
        (float)extent.width, (float)extent.height,
        0.0f, 1.0f                  // min, max depth
    );
    auto scissor = vk::Rect2D(vk::Offset2D(0, 0), extent);

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);

    vk::Buffer vertexBuffers[] = { object->getModelData()->getVertexBuffer() };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    commandBuffer.bindIndexBuffer(object->getModelData()->getIndexBuffer(), 0, vk::IndexType::eUint32);

    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        object->getPipelineLayout(),
        0,
        object->getDescriptorSets()[currentFrame],
        nullptr
    );

    commandBuffer.drawIndexed(static_cast<uint32_t>(object->getModelData()->getIndices().size()), 1, 0, 0, 0);
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
    vulkanObject* object,
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

    object->getModelData()->updateUniformBuffer(currentFrame, surface->getExtent());

    device.resetFences(1, &inFlightFences[currentFrame]);

    commandBuffers[currentFrame].reset();
    recordCommandBuffer(
        commandBuffers[currentFrame],
        object,
        renderPass,
        surface->getExtent(),
        imageIndex
    );
    
    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };

    auto submitInfo = vk::SubmitInfo(
        1, waitSemaphores, waitStages,
        1, &commandBuffers[currentFrame],
        1, signalSemaphores
    );

    try {
        instance->getGraphicsQueue().submit(submitInfo, inFlightFences[currentFrame]);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::SwapchainKHR swapChains[] = { surface->getSwapChain() };

    auto presentInfo = vk::PresentInfoKHR(
        1, signalSemaphores,
        1, swapChains,
        &imageIndex
    );

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

    if (resultPresent == vk::Result::eSuboptimalKHR || framebufferResized) {
        std::cout << "swap chain out of date/suboptimal/window resized - recreating" << std::endl;
        framebufferResized = false;
        surface->recreateSwapChain(
            renderPass,
            instance);
        return;
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}