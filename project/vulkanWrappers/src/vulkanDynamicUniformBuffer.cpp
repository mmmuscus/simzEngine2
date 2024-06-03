#include "../include/vulkanDynamicUniformBuffer.h"

vulkanDynamicUniformBuffer::~vulkanDynamicUniformBuffer() {
    destroy();
}

void vulkanDynamicUniformBuffer::destroy() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device->destroyBuffer(uniformBuffers[i]);
        uniformBuffers[i] = VK_NULL_HANDLE;

        device->freeMemory(uniformBuffersMemory[i]);
        uniformBuffersMemory[i] = VK_NULL_HANDLE;
    }
}

void vulkanDynamicUniformBuffer::init(vulkanInstance* instance) {
    device = instance->getDevice();
    initUniformBuffers(instance);
}

void vulkanDynamicUniformBuffer::initUniformBuffers(vulkanInstance* instance) {
    // Get the alignment for the dynamic uniform buffer
    // https://github.com/SaschaWillems/Vulkan/tree/master/examples/dynamicuniformbuffer
    vk::PhysicalDeviceProperties properties = instance->getPhysicalDevice().getProperties();
    size_t minAlignment = properties.limits.minUniformBufferOffsetAlignment;
    dynamicAlignment = sizeof(modelUniformBufferObject);
    if (minAlignment > 0)
        dynamicAlignment = (dynamicAlignment + minAlignment - 1) & ~(minAlignment - 1);

    vk::DeviceSize bufferSize = MAX_OBJECT_COUNT * dynamicAlignment;

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        instance->initBuffer(
            bufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            uniformBuffers[i], uniformBuffersMemory[i]
        );
    }
}

void vulkanDynamicUniformBuffer::updateModelUniformBuffer(glm::mat4 modelMat, uint32_t currentFrame, size_t objectNumber) {
    modelUniformBufferObject mbo{};
    mbo.model = modelMat;

    void* mData = device->mapMemory(
        uniformBuffersMemory[currentFrame],
        objectNumber * dynamicAlignment,
        vk::DeviceSize(sizeof(mbo))
    );
    memcpy(mData, &mbo, sizeof(mbo));
    device->unmapMemory(uniformBuffersMemory[currentFrame]);
}