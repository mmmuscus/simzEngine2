#ifndef VULKAN_INSTANCE_H_
#define VULKAN_INSTANCE_H_

#include "vulkanIncludes.h"

#include <iostream>
#include <optional>
#include <set>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return graphicsFamily.has_value();
	}
};

class vulkanInstance {
private:
	// Instance:
	vk::UniqueInstance instance;
	// Callback:
	VkDebugUtilsMessengerEXT callback;
	// Devices:
	vk::PhysicalDevice physicalDevice;
	vk::UniqueDevice device;
	// Queues:
	vk::Queue graphicsQueue;

public:
	~vulkanInstance();

	vk::UniqueInstance* getInstancePtr() { return &instance; }

	void listExtensions();
	void init();

	// Devices:
	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice);

private:
	void initInstance();
	void initCallback();
	void initPhysicalDevice();
	void initDevice();

	// Instance:
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	// Callback:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pCallback);
	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT callback,
		const VkAllocationCallbacks* pAllocator);

	// Devices:
	// TODO: ranking devices based on reqs and selecting the best one
	bool isDeviceSuitable(const vk::PhysicalDevice& device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice physDevice);
};

#endif // VULKAN_INSTANCE_H_