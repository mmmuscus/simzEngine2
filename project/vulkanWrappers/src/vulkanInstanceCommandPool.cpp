#include "../include/vulkanInstance.h"

void vulkanInstance::initCommandPool() {
	QueueFamilyIndices queueFamilyIndices =
		findQueueFamilies(physicalDevice);

	try {
		commandPool = device.get().createCommandPool(vk::CommandPoolCreateInfo(
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			queueFamilyIndices.graphicsFamily.value()
		));
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create command pool!");
	}
}

vk::CommandBuffer vulkanInstance::beginSingleTimeCommands() {
	auto allocInfo = vk::CommandBufferAllocateInfo(
		commandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	vk::CommandBuffer commandBuffer = device.get().allocateCommandBuffers(allocInfo)[0];
	auto beginInfo = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void vulkanInstance::endSingleTimeCommands(vk::CommandBuffer commandBuffer) {
	commandBuffer.end();

	auto submitInfo = vk::SubmitInfo(
		0, nullptr, nullptr,                        // wait semaphores
		1, &commandBuffer
	);

	graphicsQueue.submit(submitInfo, nullptr);
	graphicsQueue.waitIdle();

	device.get().freeCommandBuffers(commandPool, commandBuffer);
}