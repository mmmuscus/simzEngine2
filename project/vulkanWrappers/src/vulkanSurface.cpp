#include "../include/vulkanSurface.h"

vulkanSurface::~vulkanSurface() {
    for (auto imageView : imageViews) {
        device.destroyImageView(imageView);
    }

    device.destroySwapchainKHR(swapChain);

	instance.destroySurfaceKHR(surface);
}

void vulkanSurface::initSurface() {
	VkSurfaceKHR surf;

	if (glfwCreateWindowSurface(instance, window, nullptr, &surf) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");

	surface = surf;
}

void vulkanSurface::initSwapChain(vulkanInstance* inst) {
    SwapChainSupportDetails swapChainSupport = 
        inst->querySwapChainSupport(inst->getPhysicalDevice());

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D choosenExtent = chooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo(
        vk::SwapchainCreateFlagsKHR(),
        surface,
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        choosenExtent,
        1, // imageArrayLayers
        vk::ImageUsageFlagBits::eColorAttachment
    );

    QueueFamilyIndices indices = 
        inst->findQueueFamilies(inst->getPhysicalDevice());
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

    try {
        swapChain = device.createSwapchainKHR(createInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create swap chain!");
    }

    images = device.getSwapchainImagesKHR(swapChain);

    imageFormat = surfaceFormat.format;
    extent = choosenExtent;
}

void vulkanSurface::initImageViews() {
    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); i++) {
        auto createInfo = vk::ImageViewCreateInfo(
            vk::ImageViewCreateFlags(),
            images[i],
            vk::ImageViewType::e2D,
            imageFormat,
            vk::ComponentMapping(
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity
            ),
            vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor,
                0, 1, 0, 1
            )
        );

        try {
            imageViews[i] = device.createImageView(createInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void vulkanSurface::cleanupSwapChain(std::vector<vk::Framebuffer> framebuffers) {
    for (auto framebuffer : framebuffers) {
        device.destroyFramebuffer(framebuffer);
    }

    for (auto imageView : imageViews) {
        device.destroyImageView(imageView);
    }

    device.destroySwapchainKHR(swapChain);
}

void vulkanSurface::recreateSwapChain(
    vulkanRenderPass* renderPass,
    vulkanInstance* inst
) {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    device.waitIdle();

    cleanupSwapChain(renderPass->getFramebuffers());

    initSwapChain(inst);
    initImageViews();
    renderPass->initFrameBuffers(imageViews, extent);
}

vk::SurfaceFormatKHR vulkanSurface::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR vulkanSurface::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) {
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
        else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

vk::Extent2D vulkanSurface::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        vk::Extent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}
