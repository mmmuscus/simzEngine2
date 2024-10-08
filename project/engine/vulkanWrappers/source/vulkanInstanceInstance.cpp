#include "../include/vulkanInstance.h"

vulkanInstance::~vulkanInstance() {
    destroyCommandPool();
    destroyCallback();
    destroyDevice();
}

void vulkanInstance::destroyCommandPool() {
    device.destroyCommandPool(commandPool);
    commandPool = VK_NULL_HANDLE;
}

void vulkanInstance::destroyCallback() {
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, callback, nullptr);
        callback = VK_NULL_HANDLE;
    }
}

void vulkanInstance::destroyDevice() {
    device.destroy();
    device = VK_NULL_HANDLE;
}

void vulkanInstance::listExtensions() {
    std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties(nullptr);

    std::cout << "available extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}

void vulkanInstance::initInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("validation layers requested, but not available!");

    auto appInfo = vk::ApplicationInfo(
        "Momendigm Shift",
        VK_MAKE_VERSION(1, 0, 0),
        "Simz Engine",
        VK_MAKE_VERSION(2, 0, 0),
        VK_API_VERSION_1_0
    );

    auto extensions = getRequiredExtensions();

    auto createInfo = vk::InstanceCreateInfo(
        vk::InstanceCreateFlags(),
        &appInfo,
        0, nullptr, // enabled layers
        static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
    );

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    try {
        instance = vk::createInstance(createInfo, nullptr);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create instance!");
    }
}

bool vulkanInstance::checkValidationLayerSupport() {
    auto availableLayers = vk::enumerateInstanceLayerProperties();

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> vulkanInstance::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}
