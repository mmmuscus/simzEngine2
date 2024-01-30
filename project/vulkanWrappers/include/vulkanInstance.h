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
	VkDebugUtilsMessengerEXT callback = VK_NULL_HANDLE;;
	// Devices:
	vk::PhysicalDevice physicalDevice;
	vk::UniqueDevice device;
	// Queues:
	vk::Queue graphicsQueue;
	uint32_t graphicsQueueFamily;
	vk::Queue presentQueue;
	// CommandPool:
	vk::CommandPool commandPool = VK_NULL_HANDLE;

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Not maintained by the class:
	vk::SurfaceKHR surface;

public:
	~vulkanInstance();
	void destroyCallback();
	void destroyCommandPool();

	void setSurface(vk::SurfaceKHR _surface) { surface = _surface; }

	vk::Instance getInstance() { return instance.get(); }
	vk::PhysicalDevice getPhysicalDevice() { return physicalDevice; }
	vk::Device getDevice() { return device.get(); }
	vk::Queue getGraphicsQueue() { return graphicsQueue; }
	uint32_t getGraphicsQueueFamily() { return graphicsQueueFamily; }
	vk::Queue getPresentQueue() { return presentQueue; }
	vk::CommandPool getCommandPool() { return commandPool; }

	void listExtensions();

	void initInstance();
	void initCallback();
	void initPhysicalDevice();
	void initDevice();
	void initCommandPool();

	// Devices:
	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice physDevice);
	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& physDevice);
	vk::Format findSupportedFormat(
		const std::vector<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	);

	// CommandPool
	vk::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

	// Misc:
	vk::ImageView initImageView(
		vk::Image image,
		vk::Format format,
		vk::ImageAspectFlags aspectFlags,
		uint32_t mipLevels
	);

	void initBuffer(
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer, vk::DeviceMemory& bufferMemory
	);
	void initImage(
		uint32_t width, uint32_t height,
		vk::Format format,
		uint32_t mipLevels,
		vk::SampleCountFlagBits numSamples,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Image& image, vk::DeviceMemory& imageMemory
	);

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

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