#ifndef VULKAN_RENDERER_H_
#define VULKAN_RENDERER_H_

#include "vulkanIncludes.h"

#include "vulkanInstance.h"
#include "vulkanObject.h"
#include "vulkanRenderPass.h"
#include "vulkanSurface.h"

class vulkanRenderer {
private:
	// CommandPool + CommandBuffers:
	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;
	// Syncobjects:
	std::vector<vk::Semaphore> imageAvailableSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	std::vector<vk::Fence> inFlightFences;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanRenderer();

	void setDevice(vk::Device _device) { device = _device; }

	void initCommandPool(vulkanInstance* instance);
	void initCommandBuffers();
	void initSyncObjects();

	void recordCommandBuffer(
		vk::CommandBuffer  commandBuffer,
		vk::Pipeline graphicsPipeline,
		vulkanRenderPass renderPass,
		vk::Extent2D extent,
		uint32_t imageIndex
	);

	void drawFrame();

private:

};

#endif // VULKAN_RENDERER_H_