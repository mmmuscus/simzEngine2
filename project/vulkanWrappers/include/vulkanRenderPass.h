#ifndef VULKAN_RENDER_PASS_H_
#define VULKAN_RENDER_PASS_H_

#include "vulkanIncludes.h"

class vulkanRenderPass {
private:
	vk::RenderPass renderPass;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanRenderPass();

	void setDevice(vk::Device _device) { device = _device; }

	void initRenderPass(vk::Format imageFormat);

private:

};

#endif // VULKAN_RENDER_PASS_H_