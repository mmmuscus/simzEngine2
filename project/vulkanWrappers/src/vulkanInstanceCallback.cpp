#include "../include/vulkanInstance.h"

void vulkanInstance::initCallback() {
    if (!enableValidationLayers)
        return;

    auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(),
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        debugCallback,
        nullptr
    );
    
    if (CreateDebugUtilsMessengerEXT(*instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &callback) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL vulkanInstance::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
) {
    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        // Event happened that is unrelated to specification or performance
        // std::cerr << "validation layer [GENER]: " << pCallbackData->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        // Violates specification or indicates mistake
        std::cerr << "validation layer [VALID]: " << pCallbackData->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        // Potential non optimal use
        std::cerr << "validation layer [PERFO]: " << pCallbackData->pMessage << std::endl;
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
        // The implementation has modified the set of GPU visible virtual adresses associated w a Vulkan object
        std::cerr << "validation layer [MODIF]" << pCallbackData->pMessage << std::endl;
        break;
    default:
        // Shouldnt happen but: none of the above
        std::cerr << "validation layer [XXXXX]: " << pCallbackData->pMessage << std::endl;
        break;
    }

    return VK_FALSE;
}

VkResult vulkanInstance::CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pCallback
) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void vulkanInstance::DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT callback,
    const VkAllocationCallbacks* pAllocator
) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}
