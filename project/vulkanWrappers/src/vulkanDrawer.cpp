#include "../include/vulkanDrawer.h"

vulkanDrawer::~vulkanDrawer() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(renderFinishedSemaphores[i]);
        device.destroySemaphore(imageAvailableSemaphores[i]);
        device.destroyFence(inFlightFences[i]);
    }
}

void vulkanDrawer::initCommandBuffers(vk::CommandPool commandPool) {
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

void vulkanDrawer::initSyncObjects() {
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

void vulkanDrawer::getNextImage(vulkanSurface* surface) {
    device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    try {
        vk::ResultValue result = device.acquireNextImageKHR(
            surface->getSwapChain(),
            std::numeric_limits<uint64_t>::max(),
            imageAvailableSemaphores[currentFrame],
            nullptr);
        imageIndex = result.value;
    }
    catch (vk::OutOfDateKHRError err) {
        surface->setShouldRecreateSwapChain(true);
        return;
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
}

void vulkanDrawer::drawFrame(
    vulkanSurface* surface,
    vulkanRenderer* renderer,
    scene* currScene,
    vk::Queue graphicsQueue
) {
    if (surface->getShouldRecreateSwapChain())
        return;

    device.resetFences(1, &inFlightFences[currentFrame]);

    commandBuffers[currentFrame].reset();
    recordCommandBuffer(
        commandBuffers[currentFrame],
        renderer,
        surface->getExtent(),
        imageIndex,
        currScene
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
        graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

void vulkanDrawer::recordCommandBuffer(
    vk::CommandBuffer commandBuffer,
    vulkanRenderer* renderer,
    vk::Extent2D extent,
    uint32_t imageIndex,
    scene* currScene
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
        renderer->getRenderPass(),
        renderer->getFramebuffers()[imageIndex],
        renderArea,
        static_cast<uint32_t>(clearValues.size()), clearValues.data()
    );


    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    auto viewport = vk::Viewport(
        0.0f, 0.0f,                 // x, y
        (float)extent.width, (float)extent.height,
        0.0f, 1.0f                  // min, max depth
    );
    auto scissor = vk::Rect2D(vk::Offset2D(0, 0), extent);

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);

    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        currScene->getObjects()[0]->getVulkanObject()->getPipelineLayout(),
        0,
        currScene->getSceneData()->getDescriptorSets()[currentFrame],
        nullptr
    );

    for (size_t i = 0; i < currScene->getObjects().size(); i++) {
        object* currObject = currScene->getObjects()[i];
        vulkanMeshData* currMesh = currObject->getModelData()->getMeshData();

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, currObject->getVulkanObject()->getPipeline());

        vk::Buffer vertexBuffers[] = { currMesh->getVertexBuffer() };
        vk::DeviceSize offsets[] = { 0 };
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(currMesh->getIndexBuffer(), 0, vk::IndexType::eUint32);
        commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            currObject->getVulkanObject()->getPipelineLayout(),
            1,
            currObject->getModelData()->getDescriptorSets()[currentFrame],
            currObject->getObjectNumber() * static_cast<uint32_t>(currMesh->getUniformBuffer()->getDynamicAlignment())
        );
        commandBuffer.drawIndexed(
            static_cast<uint32_t>(currMesh->getIndices().size()),
            1, 0, 0, 0
        );
    }
    
    commandBuffer.endRenderPass();

    try {
        commandBuffer.end();
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void vulkanDrawer::presentFrame(vulkanSurface* surface, vk::Queue presentQueue) {
    if (surface->getShouldRecreateSwapChain())
        return;

    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    vk::SwapchainKHR swapChains[] = { surface->getSwapChain() };

    auto presentInfo = vk::PresentInfoKHR(
        1, signalSemaphores,
        1, swapChains,
        &imageIndex
    );

    vk::Result resultPresent;
    try {
        resultPresent = presentQueue.presentKHR(presentInfo);
    }
    catch (vk::OutOfDateKHRError err) {
        resultPresent = vk::Result::eErrorOutOfDateKHR;
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (resultPresent == vk::Result::eSuboptimalKHR || surface->getShouldRecreateSwapChain())
        return;

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}