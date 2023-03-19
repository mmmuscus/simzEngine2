#ifndef VULKAN_RENDERER_H_
#define VULKAN_RENDERER_H_

#include "generalIncludes.h"

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
	// Helper variables:
	size_t currentFrame = 0;
	bool framebufferResized = false;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanRenderer();

	void setFrameBufferResized(bool val) { framebufferResized = val; }
	void setDevice(vk::Device _device) { device = _device; }

	void initCommandPool(vulkanInstance* instance);
	void initCommandBuffers();
	void initSyncObjects();

	void recordCommandBuffer(
		vk::CommandBuffer commandBuffer,
		vulkanObject* object,
		vulkanRenderPass* renderPass,
		vk::Extent2D extent,
		uint32_t imageIndex
	);

	void drawFrame(
		vulkanSurface* surface,
		vulkanInstance* instance,
		vulkanObject* object,
		vulkanRenderPass* renderPass
	);

	vk::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);

private:

};

#endif // VULKAN_RENDERER_H_