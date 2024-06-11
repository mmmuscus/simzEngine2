#include "../include/imGuiInstance.h"

void imGuiInstance::recreateFramebuffers(vulkanSurface* _surface) {
	destroyFramebuffers();
	initFramebuffers(_surface);
}

void imGuiInstance::drawGui(
	scene* currScene, vulkanInstance* instance, vulkanObject* obj,
	vulkanDynamicUniformBuffer* buffer, vulkanTextureSampler* sampler
) {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// This is where the code goes that describes the Editor:
	showGui(currScene, instance, obj, buffer, sampler);

	ImGui::Render();
}

void imGuiInstance::drawFrame(vulkanSurface* _surface, uint32_t imageIndex, size_t currentFrame) {
	if (_surface->getShouldRecreateSwapChain()) return;

	auto beginInfo = vk::CommandBufferBeginInfo();

	try {
		commandBuffers[currentFrame].begin(beginInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	std::array<vk::ClearValue, 1> clearValues;
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

	auto renderPassBeginInfo = vk::RenderPassBeginInfo(
		renderPass, framebuffers[imageIndex], 
		vk::Rect2D(vk::Offset2D(0, 0), _surface->getExtent()),
		static_cast<uint32_t>(clearValues.size()), clearValues.data()
	);

	commandBuffers[currentFrame].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[currentFrame]);
	commandBuffers[currentFrame].endRenderPass();

	try {
		commandBuffers[currentFrame].end();
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to record command buffer!");
	}

	ImGui::EndFrame();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}