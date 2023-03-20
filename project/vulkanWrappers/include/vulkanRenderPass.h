#ifndef VULKAN_RENDER_PASS_H_
#define VULKAN_RENDER_PASS_H_

#include "generalIncludes.h"

class vulkanRenderPass {
private:
	// Render pass + framebuffers:
	vk::RenderPass renderPass;
	std::vector<vk::Framebuffer> framebuffers;
	// Depth resources:
	vk::Image depthImage;
	vk::DeviceMemory depthImageMemory;
	vk::ImageView depthImageView;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanRenderPass();

	void setDevice(vk::Device _device) { device = _device; }

	vk::RenderPass getRenderPass() { return renderPass; }
	std::vector<vk::Framebuffer> getFramebuffers() { return framebuffers; }

	void initRenderPass(vk::Format imageFormat);
	void initFrameBuffers(std::vector<vk::ImageView> imageViews, vk::Extent2D extent);
	void initDepthResources(vk::PhysicalDevice physicalDevice);

private:

	vk::Format findSupportedFormat(
		vk::PhysicalDevice physicalDevice,
		const std::vector<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	);

	vk::Format findDepthFormat(vk::PhysicalDevice physicalDevice);
};

#endif // VULKAN_RENDER_PASS_H_