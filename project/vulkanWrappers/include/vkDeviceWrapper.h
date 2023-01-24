#ifndef VK_DEVICE_WRAPPER_H_
#define VK_DEVICE_WRAPPER_H_

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

class vkDeviceWrapper {
private:
	VkPhysicalDevice* physicalDevice;

public:
	vkDeviceWrapper() : physicalDevice(nullptr) {}

	void init(VkInstance* instance);

private:

	bool isDeviceSuitable(VkPhysicalDevice device);
};

#endif // VK_DEVICE_WRAPPER_H_
