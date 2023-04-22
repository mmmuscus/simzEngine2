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

	// Needed for ImGui
	uint32_t imageIndex;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanDrawer();

	std::vector<vk::Semaphore> getImageAvailableSemaphores() { return imageAvailableSemaphores; }
	std::vector<vk::Semaphore> getRenderFinishedSemaphores() { return renderFinishedSemaphores; }
	std::vector<vk::Fence> getInFlightFences() { return inFlightFences; }
	size_t getCurrentFrame() { return currentFrame; }
	uint32_t getImageIndex() { return imageIndex; }

	void resetImageIndex() { imageIndex = 0; }
	void setDevice(vk::Device _device) { device = _device; }

	// void initCommandPool(vulkanInstance* instance);
	void initCommandBuffers(vk::CommandPool commandPool);
	void initSyncObjects();

	void getNextImage(vulkanSurface* surface);

	// TODO: rename function
	void drawFrame(
		vulkanSurface* surface,
		vulkanRenderer* renderer,
		scene* currScene,
		vk::Queue graphicsQueue
	);

	void recordCommandBuffer(
		vk::CommandBuffer commandBuffer,
		vulkanObject* obj,
		vulkanRenderer* renderer,
		vk::Extent2D extent,
		uint32_t imageIndex,
		scene* currScene
	);

	void presentFrame(vulkanSurface* surface, vk::Queue presentQueue);

private:

};

#endif // VULKAN_RENDERER_H_