#ifndef VULKAN_RENDERER_H_
#define VULKAN_RENDERER_H_

#include "generalIncludes.h"

#include "vulkanRenderer.h"
#include "vulkanSurface.h"

#include "../../renderLogic/include/scene.h"

class vulkanDrawer {
private:
	// CommandPool + CommandBuffers:
	// vk::CommandPool commandPool;
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
	~vulkanDrawer();

	void setFrameBufferResized(bool val) { framebufferResized = val; }
	void setDevice(vk::Device _device) { device = _device; }

	// void initCommandPool(vulkanInstance* instance);
	void initCommandBuffers(vk::CommandPool commandPool);
	void initSyncObjects();

	void recordCommandBuffer(
		vk::CommandBuffer commandBuffer,
		vulkanObject* obj,
		vulkanRenderer* renderer,
		vk::Extent2D extent,
		uint32_t imageIndex,
		scene* currScene
	);

	void drawFrame(
		vulkanSurface* surface,
		vulkanInstance* instance,
		vulkanRenderer* renderer,
		scene* currScene
	);

	// vk::CommandBuffer beginSingleTimeCommands();
	// void endSingleTimeCommands(vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue);

private:

};

#endif // VULKAN_RENDERER_H_