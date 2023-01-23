#ifndef VK_INSTANCE_WRAPPER_H_
#define VK_INSTANCE_WRAPPER_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>

class vkInstanceWrapper {
private:
	VkInstance* instance;
	VkDebugUtilsMessengerEXT* debugMessenger;

	// Validation layer variables
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
	};

	// FUNCTIONS
	bool checkValidationLayerSupport();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

	std::vector<const char*> getRequiredExtensions();

public:
	vkInstanceWrapper() : instance(nullptr), debugMessenger(nullptr) {}
	~vkInstanceWrapper();

	// TODO: change it to operator""?
	VkInstance* get() { return instance; }

	void listExtensions();
	void init();
	void setupDebugMessenger();
};

#endif /* VK_INSTANCE_WRAPPER_H_ */

