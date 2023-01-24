#ifndef VK_DEVICE_WRAPPER_H_
#define VK_DEVICE_WRAPPER_H_

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() { return graphicsFamily.has_value(); }
};

class vkDeviceWrapper {
private:
	VkPhysicalDevice* physicalDevice;

public:
	vkDeviceWrapper() : physicalDevice(nullptr) {}

	void init(VkInstance* instance);

private:

	// TODO: ranking devices based on reqs and selecting the best one
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};

#endif // VK_DEVICE_WRAPPER_H_
