#include "../include/imGuiInstance.h"

void imGuiInstance::destroyFramebuffers() {
    for (auto framebuffer : framebuffers) {
        device.destroyFramebuffer(framebuffer);
    }
}

void imGuiInstance::recreateFramebuffers(vulkanSurface* surface) {
    destroyFramebuffers();
    initFramebuffers(surface);
}

void imGuiInstance::presentGui(
    bool shouldRecreateSwapChain, scene* currScene,
    vulkanInstance* instance, vulkanObject* obj,
    meshDataManager* meshManager,
    vulkanDynamicUniformBuffer* buffer, vulkanTextureSampler* sampler
) {
    if (shouldRecreateSwapChain)
        return;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    showGui(
        currScene,
        instance, obj, 
        meshManager,
        buffer, sampler
    );
    ImGui::Render();
}

void imGuiInstance::drawFrame(
    vulkanSurface* surface,
    vulkanInstance* instance,
    uint32_t imageIndex
) {
    if (surface->getShouldRecreateSwapChain())
        return;

    vk::CommandBuffer commandBuffer = instance->beginSingleTimeCommands();

    std::array<vk::ClearValue, 1> clearValues;
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

    auto renderPassInfo = vk::RenderPassBeginInfo(
        renderPass, framebuffers[imageIndex],
        vk::Rect2D(vk::Offset2D(0, 0), surface->getExtent()),
        static_cast<uint32_t>(clearValues.size()), clearValues.data()
    );

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    commandBuffer.endRenderPass();
    instance->endSingleTimeCommands(commandBuffer);

    ImGui::EndFrame();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}