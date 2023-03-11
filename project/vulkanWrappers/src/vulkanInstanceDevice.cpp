#include "../include/vulkanInstance.h"

void vulkanInstance::initPhysicalDevice() {
    auto devices = instance->enumeratePhysicalDevices();
    if (devices.size() == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (!physicalDevice) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void vulkanInstance::initDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    float queuePriority = 1.0f;
    auto queueCreateInfo = vk::DeviceQueueCreateInfo(
        vk::DeviceQueueCreateFlags(),
        indices.graphicsFamily.value(),
        1, // queueCount
        &queuePriority
    );

    auto deviceFeatures = vk::PhysicalDeviceFeatures();
    auto createInfo = vk::DeviceCreateInfo(
        vk::DeviceCreateFlags(),
        1, &queueCreateInfo
    );
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    try {
        device = physicalDevice.createDeviceUnique(createInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create logical device!");
    }

    graphicsQueue = device->getQueue(indices.graphicsFamily.value(), 0);
}

bool vulkanInstance::isDeviceSuitable(const vk::PhysicalDevice& device) {
    QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}

QueueFamilyIndices vulkanInstance::findQueueFamilies(vk::PhysicalDevice device) {
    QueueFamilyIndices indices;

    auto queueFamilies = device.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}