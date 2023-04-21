#include "../include/vulkanTextureData.h"

vulkanTextureData::~vulkanTextureData() {
    device.destroyImageView(imageView);
    device.destroyImage(image);
    device.freeMemory(imageMemory);
}

void vulkanTextureData::init(std::string texturePath, vulkanInstance* instance) {
    device = instance->getDevice();
    initTextureImage(texturePath, instance);
    initTextureImageView(instance);
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
    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
    //mipLevels = 4;

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
        mipLevels,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        image, imageMemory
    );

    transitionImageLayout(
        image, vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout(), vk::ImageLayout::eTransferDstOptimal,
        mipLevels,
        instance
    );
    copyBufferToImage(
        stagingBuffer, image,
        static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
        instance
    );

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);

    generateMipmaps(
        image, vk::Format::eR8G8B8A8Srgb,
        texWidth, texHeight,
        mipLevels,
        instance
    );
}

void vulkanTextureData::initTextureImageView(vulkanInstance* instance) {
    imageView = instance->initImageView(image, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, mipLevels);
}

void vulkanTextureData::transitionImageLayout(
    vk::Image image, vk::Format format,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    uint32_t mipLevels,
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
            0, mipLevels,                       // basemip level, level count
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

void vulkanTextureData::generateMipmaps(
    vk::Image image, vk::Format imageFormat,
    int32_t texWidth, int32_t texHeight,
    uint32_t mipLevels,
    vulkanInstance* instance
) {
    vk::FormatProperties formatProperties =
        instance->getPhysicalDevice().getFormatProperties(imageFormat);

    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImage))
        throw std::runtime_error("texture image format does not support linear blitting!");

    vk::CommandBuffer commandBuffer = instance->beginSingleTimeCommands();

    auto barrier = vk::ImageMemoryBarrier(
        vk::AccessFlags(), vk::AccessFlags(),
        vk::ImageLayout::eUndefined, vk::ImageLayout::eUndefined,
        0, 0,
        image,
        vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eColor,
            0, 1,                               // basemip level, level count
            0, 1                                // base array layer, layer count
        )
    );

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::DependencyFlags(), 
            nullptr, nullptr, barrier
        );

        auto blit = vk::ImageBlit(
            vk::ImageSubresourceLayers(
                vk::ImageAspectFlagBits::eColor,
                i - 1,                      // mipLevel
                0, 1                        // base array layer, count
            ),
            { vk::Offset3D(0, 0, 0), vk::Offset3D(mipWidth, mipHeight, 1) },
            vk::ImageSubresourceLayers(
                vk::ImageAspectFlagBits::eColor,
                i,                          // mipLevel
                0, 1                        // base array layer, count
            ),
            {
                vk::Offset3D(0, 0, 0),
                vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1)
            }
        );

        commandBuffer.blitImage(
            image, vk::ImageLayout::eTransferSrcOptimal,
            image, vk::ImageLayout::eTransferDstOptimal,
            blit, vk::Filter::eLinear
        );

        barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlags(),
            nullptr, nullptr, barrier
        );

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::DependencyFlags(),
        nullptr, nullptr, barrier
    );

    instance->endSingleTimeCommands(commandBuffer);
}