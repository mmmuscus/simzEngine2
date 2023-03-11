#ifndef VULKAN_INCLUDES_H_
#define VULKAN_INCLUDES_H_

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

#endif // VULKAN_INCLUDES_H_