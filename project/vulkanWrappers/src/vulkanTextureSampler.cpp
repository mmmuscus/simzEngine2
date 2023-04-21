#include "../include/vulkanTextureSampler.h"

vulkanTextureSampler::~vulkanTextureSampler() {
	device.destroySampler(sampler);
}

void vulkanTextureSampler::init(vk::PhysicalDevice physicalDevice) {
    vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();

    auto samplerInfo = vk::SamplerCreateInfo(
        vk::SamplerCreateFlags(),
        vk::Filter::eLinear, vk::Filter::eLinear,   // mag, min filter
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        0,
        VK_TRUE,
        properties.limits.maxSamplerAnisotropy,
        VK_FALSE, vk::CompareOp::eAlways,           // compare op enable, compare op
        // maxLod used to be the mipmap value in textureData
        0, 4,                                       // min, max Lod
        vk::BorderColor::eIntOpaqueBlack,
        VK_FALSE
    );

    try {
        sampler = device.createSampler(samplerInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}
