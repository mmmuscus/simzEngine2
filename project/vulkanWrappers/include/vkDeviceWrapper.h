#ifndef VK_DEVICE_WRAPPER_H_
#define VK_DEVICE_WRAPPER_H_

#include "generalIncludes.h"

#include <optional>
#include <set>

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
	
	VkQueue* graphicsQueue;
	VkQueue* presentQueue;

	// The following resources are not being maintained by the class
	VkSurfaceKHR* surface;

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

public:
	vkDeviceWrapper() : 
		physicalDevice(nullptr), 
		device(nullptr), 
		graphicsQueue(nullptr), 
		presentQueue(nullptr), 
		surface(nullptr) {}
	~vkDeviceWrapper();

	VkPhysicalDevice* getPhysicalDevice() { return physicalDevice; }
	VkDevice* getDevice() { return device; }
	VkQueue* getGraphicsQueue() { return graphicsQueue; }
	VkQueue* getPresentQueue() { return presentQueue; }

	void init(const VkInstance* instance, VkSurfaceKHR* _surface);

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physDevice);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice);

private:
	void initPhysicalDevice(const VkInstance* instance);
	void initDevice();
	
	// TODO: ranking devices based on reqs and selecting the best one
	bool isDeviceSuitable(VkPhysicalDevice physDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice physDevice);
};

#endif // VK_DEVICE_WRAPPER_H_
