#ifndef VK_SYNC_WRAPPER_H_
#define VK_SYNC_WRAPPER_H_

#include "generalIncludes.h"

class vkSyncWrapper {
private:
	VkSemaphore* imageAvailableSemaphore;
	VkSemaphore* renderFinishedSemaphore;
	VkFence* inFlightFence;

public:
	vkSyncWrapper() :
		imageAvailableSemaphore(nullptr),
		renderFinishedSemaphore(nullptr),
		inFlightFence(nullptr) {}
	~vkSyncWrapper();

	void init();

private:


};

#endif // VK_SYNC_WRAPPER_H_