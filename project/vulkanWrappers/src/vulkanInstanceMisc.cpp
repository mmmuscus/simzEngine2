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