#ifndef VULKAN_RENDER_PASS_H_
#define VULKAN_RENDER_PASS_H_

#include "generalIncludes.h"

#include "vulkanInstance.h"

class vulkanRenderer {
private:
	// Render pass + framebuffers:
	vk::RenderPass renderPass = VK_NULL_HANDLE;;
	std::vector<vk::Framebuffer> framebuffers;
	// Depth resources:
	vk::Image depthImage = VK_NULL_HANDLE;;
	vk::DeviceMemory depthImageMemory = VK_NULL_HANDLE;;
	vk::ImageView depthImageView = VK_NULL_HANDLE;;
	vk::Format depthFormat;
	// MSAA Count:
	vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;
	// Color resources:
	vk::Image colorImage = VK_NULL_HANDLE;;
	vk::DeviceMemory colorImageMemory = VK_NULL_HANDLE;;
	vk::ImageView colorImageView = VK_NULL_HANDLE;;

	// Not maintained by the class:
	vk::Device* device;

public:
	~vulkanRenderer();
	void destroyFramebuffers();
	void destroyColorResources();
	void destroyDepthResources();
	void destroyRenderPass();

	void setDevice(vk::Device* _device) { device = _device; }

	vk::RenderPass getRenderPass() { return renderPass; }
	std::vector<vk::Framebuffer> getFramebuffers() { return framebuffers; }
	vk::SampleCountFlagBits getMsaaSamples() { return msaaSamples; }

	void initRenderPass(vk::Format imageFormat);
	void initFrameBuffers(std::vector<vk::ImageView> imageViews, vk::Extent2D extent);
	void initMsaaSamples(vk::PhysicalDevice physicalDevice);
	void initColorResources(vulkanInstance* instnace, vk::Format colorFormat, vk::Extent2D extent);
	void initDepthResources(vulkanInstance* instance, vk::Extent2D extent);

private:
	vk::Format findDepthFormat(vulkanInstance* instance);

	vk::SampleCountFlagBits getMaxUsableSampleCount(vk::PhysicalDevice physicalDevice);
};

#endif // VULKAN_RENDER_PASS_H_