#ifndef VK_INSTANCE_WRAPPER_H_
#define VK_INSTANCE_WRAPPER_H_

#include "generalIncludes.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

/*
A wrapper class for VkInstance, it also sets up default validation layers
The wrapper ensures RAII, so we don't need to explicitly let go of VkInstance
*/
class vkInstanceWrapper {
private:
	VkInstance* instance;
	VkDebugUtilsMessengerEXT* debugMessenger;

public:
	vkInstanceWrapper() : instance(nullptr), debugMessenger(nullptr) {}
	~vkInstanceWrapper();

	// TODO: change it to operator""?
	VkInstance* getInstance() { return instance; }

	void listExtensions();
	void init();
	void setupDebugMessenger();

private:
	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();

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
};

#endif // VK_INSTANCE_WRAPPER_H_ 

