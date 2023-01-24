#ifndef VK_DEVICE_WRAPPER_H_
#define VK_DEVICE_WRAPPER_H_

#include "generalIncludes.h"

#include <vulkan/vulkan.h>

#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() { return graphicsFamily.has_value(); }
};

class vkDeviceWrapper {
private:
	VkPhysicalDevice* physicalDevice;
	VkDevice* device;

public:
	vkDeviceWrapper() : physicalDevice(nullptr), device(nullptr) {}
	~vkDeviceWrapper();

	void init(VkInstance* instance);
	void initPhysicalDevice(VkInstance* instance);
	void initDevice();

private:
	
	// TODO: ranking devices based on reqs and selecting the best one
	bool isDeviceSuitable(VkPhysicalDevice physDevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physDevice);
};

#endif // VK_DEVICE_WRAPPER_H_
