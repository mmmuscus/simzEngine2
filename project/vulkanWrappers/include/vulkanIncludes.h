#ifndef VULKAN_INCLUDES_H_
#define VULKAN_INCLUDES_H_

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

#endif // VULKAN_INCLUDES_H_