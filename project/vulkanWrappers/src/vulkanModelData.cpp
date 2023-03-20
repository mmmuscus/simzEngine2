#include "../include/vulkanModelData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

vulkanModelData::~vulkanModelData() {
    device.destroyImage(textureImage);
    device.freeMemory(textureImageMemory);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroyBuffer(uniformBuffers[i]);
        device.freeMemory(uniformBuffersMemory[i]);
    }

    device.destroyBuffer(indexBuffer);
    device.freeMemory(indexBufferMemory);

    device.destroyBuffer(vertexBuffer);
    device.freeMemory(vertexBufferMemory);
}

void vulkanModelData::initVertexBuffer(vulkanInstance* instance) {
    vk::PhysicalDevice physicalDevice = instance->getPhysicalDevice();
    vk::Queue graphicsQueue = instance->getGraphicsQueue();

    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice,
        stagingBuffer, stagingBufferMemory
    );

    void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    device.unmapMemory(stagingBufferMemory);

    initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice,
        vertexBuffer, vertexBufferMemory
    );

    copyBuffer(
        stagingBuffer, vertexBuffer, bufferSize,
        instance
    );

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void vulkanModelData::initIndexBuffer(vulkanInstance* instance) {
    vk::PhysicalDevice physicalDevice = instance->getPhysicalDevice();
    vk::Queue graphicsQueue = instance->getGraphicsQueue();

    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice,
        stagingBuffer, stagingBufferMemory
    );

    void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), (size_t)bufferSize);
    device.unmapMemory(stagingBufferMemory);

    initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice,
        indexBuffer, indexBufferMemory
    );

    copyBuffer(
        stagingBuffer, indexBuffer, bufferSize,
        instance
    );

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void vulkanModelData::initUniformBuffers(vk::PhysicalDevice physicalDevice) {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        initBuffer(
            bufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            physicalDevice,
            uniformBuffers[i], uniformBuffersMemory[i]
        );

        uniformBuffersMapped[i] = device.mapMemory(uniformBuffersMemory[i], 0, bufferSize);
    }
}

void vulkanModelData::initTextureImage(vulkanInstance* instance) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(
        "textures/texture.jpg", 
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
    initBuffer(
        imageSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        instance->getPhysicalDevice(),
        stagingBuffer, stagingBufferMemory
    );

    void* data = device.mapMemory(stagingBufferMemory, 0, imageSize);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    device.unmapMemory(stagingBufferMemory);
    stbi_image_free(pixels);

    initImage(
        texWidth, texHeight,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        textureImage, textureImageMemory,
        instance->getPhysicalDevice()
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

void vulkanModelData::initBuffer(
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::PhysicalDevice physicalDevice,
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
        findMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice)
    );

    try {
        bufferMemory = device.allocateMemory(allocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void vulkanModelData::copyBuffer(
    vk::Buffer src, vk::Buffer dst, vk::DeviceSize size,
    vulkanInstance* instance
) {
    vk::CommandBuffer commandBuffer = instance->beginSingleTimeCommands();

    auto copyRegion = vk::BufferCopy(0, 0, size);   // src, dst offset
    commandBuffer.copyBuffer(src, dst, copyRegion);

    instance->endSingleTimeCommands(commandBuffer);
}

void vulkanModelData::initImage(
    uint32_t width, uint32_t height,
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::Image& image, vk::DeviceMemory& imageMemory,
    vk::PhysicalDevice physicalDevice
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
        image = device.createImage(imageInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error("failed to create image!");
    }

    vk::MemoryRequirements memRequirements;
    device.getImageMemoryRequirements(image, &memRequirements);
    
    auto allocInfo = vk::MemoryAllocateInfo(
        memRequirements.size,
        findMemoryType(
            memRequirements.memoryTypeBits, 
            properties,
            physicalDevice
        )
    );

    try {
        imageMemory = device.allocateMemory(allocInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    device.bindImageMemory(image, imageMemory, 0);
}

void vulkanModelData::transitionImageLayout(
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

void vulkanModelData::copyBufferToImage(
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

uint32_t vulkanModelData::findMemoryType(
    uint32_t typeFilter,
    vk::MemoryPropertyFlags properties,
    vk::PhysicalDevice physicalDevice
) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void vulkanModelData::updateUniformBuffer(uint32_t currentImage, vk::Extent2D extent) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = 
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime)
        .count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}