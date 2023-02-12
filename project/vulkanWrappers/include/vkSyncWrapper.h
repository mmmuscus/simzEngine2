#ifndef VK_SYNC_WRAPPER_H_
#define VK_SYNC_WRAPPER_H_

#include "generalIncludes.h"

class vkSyncWrapper {
private:
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	VkDevice* device;

public:
	vkSyncWrapper() : device(nullptr) {}
	~vkSyncWrapper();

	std::vector<VkSemaphore> getImageAvailableSemaphore() { return imageAvailableSemaphores; }
	std::vector<VkSemaphore> getRenderFinishedSemaphore() { return renderFinishedSemaphores; }
	std::vector<VkFence> getInFlightFence() { return inFlightFences; }

	void init(VkDevice* _device);

private:


};

#endif // VK_SYNC_WRAPPER_H_