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
	// MSAA Count:
	vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;
	// Color resources:
	vk::Image colorImage;
	vk::DeviceMemory colorImageMemory;
	vk::ImageView colorImageView;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanRenderPass();

	void setDevice(vk::Device _device) { device = _device; }

	vk::RenderPass getRenderPass() { return renderPass; }
	std::vector<vk::Framebuffer> getFramebuffers() { return framebuffers; }
	vk::SampleCountFlagBits getMsaaSamples() { return msaaSamples; }

	void initRenderPass(vulkanInstance* instance, vk::Format imageFormat);
	void initFrameBuffers(std::vector<vk::ImageView> imageViews, vk::Extent2D extent);
	void initMsaaSamples(vk::PhysicalDevice physicalDevice);
	void initColorResources(vulkanInstance* instnace, vk::Format colorFormat, vk::Extent2D extent);
	void initDepthResources(vulkanInstance* instance, vk::Extent2D extent);

	void destroyFramebuffers();
	void destroyColorResources();
	void destroyDepthResources();

private:
	vk::Format findDepthFormat(vulkanInstance* instance);

	vk::SampleCountFlagBits getMaxUsableSampleCount(vk::PhysicalDevice physicalDevice);
};

#endif // VULKAN_RENDER_PASS_H_