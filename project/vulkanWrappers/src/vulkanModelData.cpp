#include "../include/vulkanModelData.h"

vulkanModelData::~vulkanModelData() {
    device.destroyBuffer(indexBuffer);
    device.freeMemory(indexBufferMemory);

    device.destroyBuffer(vertexBuffer);
    device.freeMemory(vertexBufferMemory);
}

void vulkanModelData::initVertexBuffer(vulkanInstance* instance, vk::CommandPool commandPool) {
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
        stagingBuffer,
        stagingBufferMemory
    );

    void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    device.unmapMemory(stagingBufferMemory);

    initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice,
        vertexBuffer,
        vertexBufferMemory
    );

    copyBuffer(
        stagingBuffer, vertexBuffer, bufferSize,
        commandPool, graphicsQueue
    );

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void vulkanModelData::initIndexBuffer(vulkanInstance* instance, vk::CommandPool commandPool) {
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
        stagingBuffer,
        stagingBufferMemory
    );

    void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), (size_t)bufferSize);
    device.unmapMemory(stagingBufferMemory);

    initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice,
        indexBuffer,
        indexBufferMemory
    );

    copyBuffer(
        stagingBuffer, indexBuffer, bufferSize,
        commandPool, graphicsQueue
    );

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void vulkanModelData::initBuffer(
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::PhysicalDevice physicalDevice,
    vk::Buffer& buffer,
    vk::DeviceMemory& bufferMemory
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
    vk::CommandPool commandPool, vk::Queue graphicsQueue
) {
    auto allocInfo = vk::CommandBufferAllocateInfo(
        commandPool,
        vk::CommandBufferLevel::ePrimary,
        1
    );

    vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo)[0];
    auto beginInfo = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    commandBuffer.begin(beginInfo);
    auto copyRegion = vk::BufferCopy(0, 0, size);   // src, dst offset
    commandBuffer.copyBuffer(src, dst, copyRegion);
    commandBuffer.end();

    auto submitInfo = vk::SubmitInfo(
        0, nullptr, nullptr,                        // wait semaphores
        1, &commandBuffer
    );

    graphicsQueue.submit(submitInfo, nullptr);
    graphicsQueue.waitIdle();

    device.freeCommandBuffers(commandPool, commandBuffer);
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