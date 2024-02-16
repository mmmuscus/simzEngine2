#include "../include/imGuiInstance.h"

void imGuiInstance::recreateFramebuffers(vulkanSurface* _surface) {
	destroyFramebuffers();
	initFramebuffers(_surface);
}

void imGuiInstance::drawGui() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	ImGui::Render();
}

void imGuiInstance::drawFrame(vk::CommandBuffer _commandBuffer, vulkanSurface* _surface, uint32_t imageIndex) {
	if (_surface->getShouldRecreateSwapChain()) return;
	
	// vk::CommandBuffer commandBuffer = _instance->beginSingleTimeCommands();
	auto beginInfo = vk::CommandBufferBeginInfo();

	try {
		_commandBuffer.begin(beginInfo);
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

	_commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _commandBuffer);
	_commandBuffer.endRenderPass();
	// _instance->endSingleTimeCommands(commandBuffer);

	try {
		_commandBuffer.end();
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to record command buffer!");
	}

	ImGui::EndFrame();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}