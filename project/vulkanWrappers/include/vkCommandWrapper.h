#ifndef VK_COMMAND_WRAPPER_H_
#define VK_COMMAND_WRAPPER_H_

#include "generalIncludes.h"

#include "vkDeviceWrapper.h"
#include "vkSwapChainWrapper.h"
#include "vkPipelineWrapper.h"

class vkCommandWrapper {
private:
	VkCommandPool* commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	// The following resources are not being maintained by the class
	VkDevice* device;

public:
	vkCommandWrapper() : commandPool(nullptr) {}
	~vkCommandWrapper();

	std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; }
	VkCommandBuffer getCommandBuffer(uint32_t i) { return commandBuffers[i]; }

	void init(vkDeviceWrapper* deviceWrapper);

	void recordCommandBuffer(
		VkCommandBuffer commandBuffer, 
		vkPipelineWrapper* pipelineWrapper, 
		vkSwapChainWrapper* swapChainWrapper, 
		uint32_t imageIndex);

private:
	void initCommandPool(vkDeviceWrapper* deviceWrapper);
	void initCommandBuffer();

};

#endif // VK_COMMAND_WRAPPER_H_