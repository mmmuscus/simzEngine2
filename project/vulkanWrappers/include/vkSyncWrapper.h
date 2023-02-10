#ifndef VK_SYNC_WRAPPER_H_
#define VK_SYNC_WRAPPER_H_

#include "generalIncludes.h"

class vkSyncWrapper {
private:
	VkSemaphore* imageAvailableSemaphore;
	VkSemaphore* renderFinishedSemaphore;
	VkFence* inFlightFence;

	VkDevice* device;

public:
	vkSyncWrapper() :
		imageAvailableSemaphore(nullptr),
		renderFinishedSemaphore(nullptr),
		inFlightFence(nullptr),
		device(nullptr) {}
	~vkSyncWrapper();

	VkSemaphore* getImageAvailableSemaphore() { return imageAvailableSemaphore; }
	VkSemaphore* getRenderFinishedSemaphore() { return renderFinishedSemaphore; }
	VkFence* getInFlightFence() { return inFlightFence; }

	void init(VkDevice* _device);

private:


};

#endif // VK_SYNC_WRAPPER_H_