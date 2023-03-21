#include "../include/vulkanTextureData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

vulkanTextureData::~vulkanTextureData() {
    device.destroySampler(textureSampler);
    device.destroyImageView(textureImageView);
    device.destroyImage(textureImage);
    device.freeMemory(textureImageMemory);
}

void vulkanTextureData::initTextureImage(std::string texturePath, vulkanInstance* instance) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(
        texturePath.c_str(),
        &texWidth,
        &texHeight,
        &texChannels,
        STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    instance->initBuffer(
        imageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory
    );

    void* data = device.mapMemory(stagingBufferMemory, 0, imageSize);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    device.unmapMemory(stagingBufferMemory);
    stbi_image_free(pixels);

    instance->initImage(
        texWidth, texHeight,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        textureImage, textureImageMemory
    );

    transitionImageLayout(
        textureImage, vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout(), vk::ImageLayout::eTransferDstOptimal,
        instance
    );
    copyBufferToImage(
        stagingBuffer, textureImage,
        static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
        instance
    );
    transitionImageLayout(
        textureImage, vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
        instance
    );

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void vulkanTextureData::initTextureImageView(vulkanInstance* instance) {
    textureImageView = instance->initImageView(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void vulkanTextureData::initTextureSampler(vk::PhysicalDevice physicalDevice) {
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
        0, 0,                                       // min, max Lod
        vk::BorderColor::eIntOpaqueBlack,
        VK_FALSE
    );

    try {
        textureSampler = device.createSampler(samplerInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void vulkanTextureData::transitionImageLayout(
    vk::Image image, vk::Format format,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    vulkanInstance* instance
) {
    vk::CommandBuffer commandBuffer = instance->beginSingleTimeCommands();

    auto barrier = vk::ImageMemoryBarrier(
        vk::AccessFlags(), vk::AccessFlags(),   // temp values
        oldLayout, newLayout,
        0, 0,
        image,
        vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eColor,
            0, 1,                               // basemip level, level count
            0, 1                                // base array layer, layer count
        )
    );

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout() && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(
        sourceStage, destinationStage,
        vk::DependencyFlags(),
        nullptr,
        nullptr,
        barrier
    );

    instance->endSingleTimeCommands(commandBuffer);
}

void vulkanTextureData::copyBufferToImage(
    vk::Buffer buffer, vk::Image image,
    uint32_t width, uint32_t height,
    vulkanInstance* instance
) {
    vk::CommandBuffer commandBuffer = instance->beginSingleTimeCommands();

    auto region = vk::BufferImageCopy(
        0,                                  // buffer offset
        0, 0,                               // row length, image height
        vk::ImageSubresourceLayers(
            vk::ImageAspectFlagBits::eColor,
            0,                              // mip level
            0, 1                            // base array layer, count
        ),
        vk::Offset3D(0, 0, 0),
        vk::Extent3D(width, height, 1)
    );

    commandBuffer.copyBufferToImage(
        buffer, image,
        vk::ImageLayout::eTransferDstOptimal,
        region
    );

    instance->endSingleTimeCommands(commandBuffer);
}