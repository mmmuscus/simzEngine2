#ifndef VULKAN_TEXTURE_SAMPLER_H_
#define VULKAN_TEXTURE_SAMPLER_H_

#include "generalIncludes.h"

class vulkanTextureSampler {
private:
	vk::Sampler sampler = VK_NULL_HANDLE;

	// Not maintained by the class:
	vk::Device* device;

public:
	~vulkanTextureSampler();
	void destroy();

	void setDevice(vk::Device* _device) { device = _device; }

	vk::Sampler get() { return sampler; }

	void init(vk::Device* _device, vk::PhysicalDevice physicalDevice);

private:
	void initSampler(vk::PhysicalDevice physicalDevice);

};

#endif // VULKAN_TEXTURE_SAMPLER_H_
