#ifndef VULKAN_GENERAL_INCLUDES_H_
#define VULKAN_GENERAL_INCLUDES_H_

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <optional>

#include "../../general/include/generalIncludes.h"

#define VALIDATION_LAYERS_ENABLED

#ifdef VALIDATION_LAYERS_ENABLED
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif // VALIDATION_LAYERS_ENABLED

const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

const int MAX_FRAMES_IN_FLIGHT = 2;

const int MAX_OBJECT_COUNT = 100;

const uint32_t MAX_DESCRIPTORPOOL_SETS = 10000;

#endif // VULKAN_GENERAL_INCLUDES_H_