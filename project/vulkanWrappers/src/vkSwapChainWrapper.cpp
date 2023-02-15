#include "../include/vkSwapChainWrapper.h"

vkSwapChainWrapper::~vkSwapChainWrapper() {
    cleanupSwapChain();
}

void vkSwapChainWrapper::init(vkDeviceWrapper* deviceWrapper, GLFWwindow* window, VkSurfaceKHR* surface) {
    device = deviceWrapper->getDevice();
    
    initSwapChain(deviceWrapper, window, surface);
    initImageViews();
}

void vkSwapChainWrapper::recreateSwapChain(
    vkDeviceWrapper* deviceWrapper, 
    GLFWwindow* window, 
    VkSurfaceKHR* surface,
    VkRenderPass* renderPass
) {
    vkDeviceWaitIdle(*device);

    initSwapChain(deviceWrapper, window, surface);
    initImageViews();
    initFrameBuffers(renderPass);
}

void vkSwapChainWrapper::cleanupSwapChain() {
    if (device == nullptr)
        return;

    for (size_t i = 0; i < framebuffers.size(); i++)
        vkDestroyFramebuffer(*device, framebuffers[i], nullptr);

    for (size_t i = 0; i < imageViews.size(); i++)
        vkDestroyImageView(*device, imageViews[i], nullptr);

    vkDestroySwapchainKHR(*device, *swapChain, nullptr);
}

void vkSwapChainWrapper::initSwapChain(vkDeviceWrapper* deviceWrapper, GLFWwindow* window, VkSurfaceKHR* surface) {
    swapChain = new VkSwapchainKHR();

    SwapChainSupportDetails swapChainSupport =
        deviceWrapper->querySwapChainSupport(*deviceWrapper->getPhysicalDevice());

    VkSurfaceFormatKHR chosenSurfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR chosenPresentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D chosenExtent = chooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = *surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = chosenSurfaceFormat.format;
    createInfo.imageColorSpace = chosenSurfaceFormat.colorSpace;
    createInfo.imageExtent = chosenExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices =
        deviceWrapper->findQueueFamilies(*deviceWrapper->getPhysicalDevice());
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = chosenPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(*device, &createInfo, nullptr, swapChain) != VK_SUCCESS)
        throw std::runtime_error("failed to create swap chain!");

    vkGetSwapchainImagesKHR(*device, *swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(*device, *swapChain, &imageCount, images.data());

    imageFormat = new VkFormat(chosenSurfaceFormat.format);
    extent = new VkExtent2D(chosenExtent);
}

void vkSwapChainWrapper::initImageViews() {
    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = *imageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(*device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create image views!");
    }
}

void vkSwapChainWrapper::initFrameBuffers(VkRenderPass* renderPass) {
    framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        VkImageView attachments[] = {
            imageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = *renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent->width;
        framebufferInfo.height = extent->height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(*device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

VkSurfaceFormatKHR vkSwapChainWrapper::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR vkSwapChainWrapper::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vkSwapChainWrapper::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}