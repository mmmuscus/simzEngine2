#ifndef VK_DEVICE_WRAPPER_H_
#define VK_DEVICE_WRAPPER_H_

#include "generalIncludes.h"

#include <vulkan/vulkan.h>

#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};
 /*
 A wrapper class for VkPhysicalDevice and VkDevice, handled at the same place for convinience
 The wrapper ensures RAII, so we don't need to explicitly let go of VkDevice
 */
class vkDeviceWrapper {
private:
	VkPhysicalDevice* physicalDevice;
	VkDevice* device;

	VkSurfaceKHR* surface;

public:
	vkDeviceWrapper() : physicalDevice(nullptr), device(nullptr) {}
	~vkDeviceWrapper();

	void init(const VkInstance* instance, VkSurfaceKHR* _surface);
	void initPhysicalDevice(const VkInstance* instance);
	void initDevice();

private:
	
	// TODO: ranking devices based on reqs and selecting the best one
	bool isDeviceSuitable(VkPhysicalDevice physDevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physDevice);
};

#endif // VK_DEVICE_WRAPPER_H_
