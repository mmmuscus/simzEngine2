#ifndef VK_COMMAND_WRAPPER_H_
#define VK_COMMAND_WRAPPER_H_

#include "generalIncludes.h"

#include "vkDeviceWrapper.h"

class vkCommandWrapper {
private:
	VkCommandPool* commandPool;
	VkCommandBuffer* commandBuffer;

	// The following resources are not being maintained by the class
	VkDevice* device;

public:
	vkCommandWrapper() : commandPool(nullptr) {}
	~vkCommandWrapper();

	void init(vkDeviceWrapper* deviceWrapper);

private:
	void initCommandPool(vkDeviceWrapper* deviceWrapper);
	void initCommandBuffer();

};

#endif // VK_COMMAND_WRAPPER_H_