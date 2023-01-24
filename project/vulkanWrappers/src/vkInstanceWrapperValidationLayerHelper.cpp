#include "../include/vkInstanceWrapper.h"

VKAPI_ATTR VkBool32 VKAPI_CALL vkInstanceWrapper::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer ";

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            // Event happened that is unrelated to specification or performance
            std::cerr << "[GENER]: " << pCallbackData->pMessage << std::endl;
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            // Violates specification or indicates mistake
            std::cerr << "[VALID]: " << pCallbackData->pMessage << std::endl;
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            // Potential non optimal use
            std::cerr << "[PERFO]: " << pCallbackData->pMessage << std::endl;
            break;
        default:
            // Shouldnt happen but: none of the above
            std::cerr << "[XXXXX]: " << pCallbackData->pMessage << std::endl;
            break;
    }

    return VK_FALSE;
}

VkResult vkInstanceWrapper::CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void vkInstanceWrapper::DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}