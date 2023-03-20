#include "../include/vulkanInstance.h"

vk::ImageView vulkanInstance::initImageView(
	vk::Image image, 
	vk::Format format, 
	vk::ImageAspectFlags aspectFlags
) {
	auto viewInfo = vk::ImageViewCreateInfo(
		vk::ImageViewCreateFlags(),
		image,
		vk::ImageViewType::e2D,
		format,
		vk::ComponentMapping(),
		vk::ImageSubresourceRange(
			aspectFlags,
			0, 1,							// base mip level, count
			0, 1							// base array layer, count
		)
	);

	vk::ImageView imageView;
	try {
		imageView = device.get().createImageView(viewInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

/*void vulkanInstance::initImage(
	uint32_t width, uint32_t height,
	vk::Format format,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage,
	vk::MemoryPropertyFlags properties,
	vk::Image& image, vk::DeviceMemory& imageMemory
) {
    auto imageInfo = vk::ImageCreateInfo(
        vk::ImageCreateFlags(),
        vk::ImageType::e2D,
        format,
        vk::Extent3D(width, height, 1),
        1, 1,                               // mip, array levels
        vk::SampleCountFlagBits::e1,
        tiling,
        usage,
        vk::SharingMode::eExclusive,
        0, nullptr,                         // queue family inex count, indices
        vk::ImageLayout::eUndefined
    );

    try {
        image = device.get().createImage(imageInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create image!");
    }

    vk::MemoryRequirements memRequirements;
    device.get().getImageMemoryRequirements(image, &memRequirements);

    auto allocInfo = vk::MemoryAllocateInfo(
        memRequirements.size,
        findMemoryType(
            memRequirements.memoryTypeBits,
            properties,
            physicalDevice
        )
    );

    try {
        imageMemory = device.get().allocateMemory(allocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    device.get().bindImageMemory(image, imageMemory, 0);
}*/