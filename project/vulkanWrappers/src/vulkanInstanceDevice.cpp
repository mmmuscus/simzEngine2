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

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        queueCreateInfos.push_back({
            vk::DeviceQueueCreateFlags(),
            queueFamily,
            1, // queueCount
            &queuePriority
        });
    }

    auto deviceFeatures = vk::PhysicalDeviceFeatures();
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;
    auto createInfo = vk::DeviceCreateInfo(
        vk::DeviceCreateFlags(),
        static_cast<uint32_t>(queueCreateInfos.size()),
        queueCreateInfos.data()
    );
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    try {
        device = physicalDevice.createDevice(createInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create logical device!");
    }

    graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
    graphicsQueueFamily = indices.graphicsFamily.value();

    presentQueue = device.getQueue(indices.presentFamily.value(), 0);
}

bool vulkanInstance::isDeviceSuitable(const vk::PhysicalDevice& physDevice) {
    QueueFamilyIndices indices = findQueueFamilies(physDevice);

    bool extensionsSupported = checkDeviceExtensionSupport(physDevice);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physDevice);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    vk::PhysicalDeviceFeatures supportedFeatures = physDevice.getFeatures();

    return indices.isComplete() && 
        extensionsSupported && 
        swapChainAdequate && 
        supportedFeatures.samplerAnisotropy;
}

bool vulkanInstance::checkDeviceExtensionSupport(const vk::PhysicalDevice& physDevice) {
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : physDevice.enumerateDeviceExtensionProperties()) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices vulkanInstance::findQueueFamilies(vk::PhysicalDevice physDevice) {
    QueueFamilyIndices indices;

    auto queueFamilies = physDevice.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;

        if (queueFamily.queueCount > 0 && physDevice.getSurfaceSupportKHR(i, surface))
            indices.presentFamily = i;

        if (indices.isComplete())
            break;

        i++;
    }

    return indices;
}

SwapChainSupportDetails vulkanInstance::querySwapChainSupport(const vk::PhysicalDevice& physDevice) {
    SwapChainSupportDetails details;
    details.capabilities = physDevice.getSurfaceCapabilitiesKHR(surface);
    details.formats = physDevice.getSurfaceFormatsKHR(surface);
    details.presentModes = physDevice.getSurfacePresentModesKHR(surface);

    return details;
}

vk::Format vulkanInstance::findSupportedFormat(
    const std::vector<vk::Format>& candidates,
    vk::ImageTiling tiling,
    vk::FormatFeatureFlags features
) {
    for (vk::Format format : candidates) {
        vk::FormatProperties properties = physicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
            return format;
        else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
            return format;

        throw std::runtime_error("failed to find supported format!");
    }
}