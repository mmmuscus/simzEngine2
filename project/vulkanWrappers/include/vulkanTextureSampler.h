#ifndef VULKAN_TEXTURE_SAMPLER_H_
#define VULKAN_TEXTURE_SAMPLER_H_

#include "generalIncludes.h"

class vulkanTextureSampler {
private:
	vk::Sampler sampler;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanTextureSampler();

	void setDevice(vk::Device _device) { device = _device; }

	vk::Sampler get() { return sampler; }

	void init(vk::PhysicalDevice physicalDevice);

private:

};

#endif // VULKAN_TEXTURE_SAMPLER_H_