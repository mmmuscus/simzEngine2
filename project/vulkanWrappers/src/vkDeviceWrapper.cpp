#include "../include/vkDeviceWrapper.h"

vkDeviceWrapper::~vkDeviceWrapper() {
	vkDestroyDevice(*device, nullptr);
}

void vkDeviceWrapper::init(const VkInstance* instance, VkSurfaceKHR* _surface) {
	surface = _surface;

	initPhysicalDevice(instance);
	initDevice();
}

void vkDeviceWrapper::initPhysicalDevice(const VkInstance* instance) {
	physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

	for (const auto& physDevice : devices) {
		if (isDeviceSuitable(physDevice)) {
			physicalDevice = new VkPhysicalDevice(physDevice);
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("failed to find a suitable GPU!");
}

void vkDeviceWrapper::initDevice() {
	device = new VkDevice();

	QueueFamilyIndices indices = findQueueFamilies(*physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	// We select the queried features we queried with vkGetPhysicalDeviceFeatures
	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	/*
	Previous implementations of Vulkan made distinctions between instance and device specific
	validation layers, this is no longer the case its a good idea to set them up to be 
	compatible with previous implementations
	*/
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(*physicalDevice, &createInfo, nullptr, device) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device!");
}

bool vkDeviceWrapper::isDeviceSuitable(VkPhysicalDevice physDevice) {
	QueueFamilyIndices indices = findQueueFamilies(physDevice);
	return indices.isComplete();
}

QueueFamilyIndices vkDeviceWrapper::findQueueFamilies(VkPhysicalDevice physDevice) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
			indices.graphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, *surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}