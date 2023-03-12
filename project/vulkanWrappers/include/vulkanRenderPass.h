#ifndef VULKAN_RENDER_PASS_H_
#define VULKAN_RENDER_PASS_H_

#include "generalIncludes.h"

class vulkanRenderPass {
private:
	vk::RenderPass renderPass;
	std::vector<vk::Framebuffer> framebuffers;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanRenderPass();

	void setDevice(vk::Device _device) { device = _device; }

	vk::RenderPass getRenderPass() { return renderPass; }
	std::vector<vk::Framebuffer> getFramebuffers() { return framebuffers; }

	void initRenderPass(vk::Format imageFormat);
	void initFrameBuffers(std::vector<vk::ImageView> imageViews, vk::Extent2D extent);

private:

};

#endif // VULKAN_RENDER_PASS_H_