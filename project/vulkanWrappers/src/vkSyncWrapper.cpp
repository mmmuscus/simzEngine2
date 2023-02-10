#include "../include/vkSyncWrapper.h"

vkSyncWrapper::~vkSyncWrapper() {
	if (device == nullptr)
		return;

	if (imageAvailableSemaphore != nullptr)
		vkDestroySemaphore(*device, *imageAvailableSemaphore, nullptr);

	if (renderFinishedSemaphore != nullptr)
		vkDestroySemaphore(*device, *renderFinishedSemaphore, nullptr);

	if (inFlightFence != nullptr)
		vkDestroyFence(*device, *inFlightFence, nullptr);
}

void vkSyncWrapper::init(VkDevice* _device) {
	device = _device;

	imageAvailableSemaphore = new VkSemaphore();
	renderFinishedSemaphore = new VkSemaphore();
	inFlightFence = new VkFence();

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	if (vkCreateSemaphore(*device, &semaphoreInfo, nullptr, imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(*device, &semaphoreInfo, nullptr, renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(*device, &fenceInfo, nullptr, inFlightFence) != VK_SUCCESS)
		throw std::runtime_error("failed to create semaphores!");
}