#include "../include/vkCommandWrapper.h"

vkCommandWrapper::~vkCommandWrapper() {
	if (device == nullptr)
		return;

	if (commandPool != nullptr)
		vkDestroyCommandPool(*device, *commandPool, nullptr);
}

void vkCommandWrapper::init(vkDeviceWrapper* deviceWrapper) {
	device = deviceWrapper->getDevice();

	initCommandPool(deviceWrapper);
}

void vkCommandWrapper::initCommandPool(vkDeviceWrapper* deviceWrapper) {
	commandPool = new VkCommandPool();

	QueueFamilyIndices queueFamilyIndices = 
		deviceWrapper->findQueueFamilies(*deviceWrapper->getPhysicalDevice());

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(*device, &poolInfo, nullptr, commandPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");
}

void vkCommandWrapper::initCommandBuffer() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = *commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(*device, &allocInfo, commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");
}