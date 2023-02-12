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

	std::vector<VkSemaphore> getImageAvailableSemaphores() { return imageAvailableSemaphores; }
	std::vector<VkSemaphore> getRenderFinishedSemaphores() { return renderFinishedSemaphores; }
	std::vector<VkFence> getInFlightFences() { return inFlightFences; }

	void init(VkDevice* _device);

private:


};

#endif // VK_SYNC_WRAPPER_H_