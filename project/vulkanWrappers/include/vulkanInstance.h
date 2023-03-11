#ifndef VULKAN_INSTANCE_H_
#define VULKAN_INSTANCE_H_

#include "vulkanIncludes.h"

#include <iostream>
#include <optional>
#include <set>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
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
	vk::Queue presentQueue;

	// Not maintained by the class:
	vk::SurfaceKHR surface;

public:
	~vulkanInstance();

	void setSurface(vk::SurfaceKHR _surface) { surface = _surface; }

	vk::Instance getInstance() { return instance.get(); }

	void listExtensions();

	void initInstance();
	void initCallback();
	void initPhysicalDevice();
	void initDevice();

	// Devices:
	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice);

private:
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