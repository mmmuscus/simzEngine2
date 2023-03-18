#include "../include/vulkanModelData.h"

vulkanModelData::~vulkanModelData() {
    for (size_t i; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroyBuffer(uniformBuffers[i]);
        device.freeMemory(uniformBuffersMemory[i]);
    }

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
        commandPool, graphicsQueue
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