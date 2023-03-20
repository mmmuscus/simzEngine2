#ifndef VULKAN_RENDER_PASS_H_
#define VULKAN_RENDER_PASS_H_

#include "generalIncludes.h"

#include "vulkanInstance.h"

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

	void initRenderPass(vulkanInstance* instance, vk::Format imageFormat);
	void initFrameBuffers(std::vector<vk::ImageView> imageViews, vk::Extent2D extent);
	void initDepthResources(vulkanInstance* instance, vk::Extent2D extent);

	void destroyFramebuffers();
	void destroyDepthResources();

private:
	vk::Format findDepthFormat(vulkanInstance* instance);
};

#endif // VULKAN_RENDER_PASS_H_