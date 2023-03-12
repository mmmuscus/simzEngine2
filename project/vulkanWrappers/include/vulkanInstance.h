#ifndef VULKAN_INSTANCE_H_
#define VULKAN_INSTANCE_H_

#include "generalIncludes.h"

#include <iostream>
#include <set>

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

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Not maintained by the class:
	vk::SurfaceKHR surface;

public:
	~vulkanInstance();

	void setSurface(vk::SurfaceKHR _surface) { surface = _surface; }

	vk::Instance getInstance() { return instance.get(); }
	vk::PhysicalDevice getPhysicalDevice() { return physicalDevice; }
	vk::Device getDevice() { return device.get(); }
	vk::Queue getGraphicsQueue() { return graphicsQueue; }
	vk::Queue getPresentQueue() { return presentQueue; }

	void listExtensions();

	void initInstance();
	void initCallback();
	void initPhysicalDevice();
	void initDevice();

	// Devices:
	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice physDevice);
	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& physDevice);

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
	bool isDeviceSuitable(const vk::PhysicalDevice& physDevice);
	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& physDevice);
};

#endif // VULKAN_INSTANCE_H_