#include "../include/vulkanInstance.h"

vk::ImageView vulkanInstance::initImageView(
	vk::Image image, 
	vk::Format format, 
	vk::ImageAspectFlags aspectFlags,
    uint32_t mipLevels
) {
	auto viewInfo = vk::ImageViewCreateInfo(
		vk::ImageViewCreateFlags(),
		image,
		vk::ImageViewType::e2D,
		format,
		vk::ComponentMapping(),
		vk::ImageSubresourceRange(
			aspectFlags,
			0, mipLevels,				    // base mip level, count
			0, 1							// base array layer, count
		)
	);

	vk::ImageView imageView;
	try {
		imageView = device.createImageView(viewInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

void vulkanInstance::initBuffer(
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::Buffer& buffer, vk::DeviceMemory& bufferMemory
) {
    auto bufferInfo = vk::BufferCreateInfo(
        vk::BufferCreateFlags(),
        size, usage, vk::SharingMode::eExclusive
    );

    try {
        buffer = device.createBuffer(bufferInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create buffer!");
    }

    vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(buffer);

    auto allocInfo = vk::MemoryAllocateInfo(
        memRequirements.size,
        findMemoryType(memRequirements.memoryTypeBits, properties)
    );

    try {
        bufferMemory = device.allocateMemory(allocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void vulkanInstance::initImage(
	uint32_t width, uint32_t height,
	vk::Format format,
    uint32_t mipLevels,
    vk::SampleCountFlagBits numSamples,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage,
	vk::MemoryPropertyFlags properties,
    vk::ImageLayout initialLayout,
	vk::Image& image, vk::DeviceMemory& imageMemory
) {
    auto imageInfo = vk::ImageCreateInfo(
        vk::ImageCreateFlags(),
        vk::ImageType::e2D,
        format,
        vk::Extent3D(width, height, 1),
        mipLevels, 1,                       // mip, array levels
        numSamples,
        tiling,
        usage,
        vk::SharingMode::eExclusive,
        0, nullptr,                         // queue family inex count, indices
        initialLayout
    );

    try {
        image = device.createImage(imageInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create image!");
    }

    vk::MemoryRequirements memRequirements;
    device.getImageMemoryRequirements(image, &memRequirements);

    auto allocInfo = vk::MemoryAllocateInfo(
        memRequirements.size,
        findMemoryType(memRequirements.memoryTypeBits, properties)
    );

    try {
        imageMemory = device.allocateMemory(allocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    device.bindImageMemory(image, imageMemory, 0);
}

uint32_t vulkanInstance::findMemoryType( uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}