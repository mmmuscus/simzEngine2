#include "../include/vulkanSceneData.h"

vulkanSceneData::~vulkanSceneData() {
    destroy();
}

void vulkanSceneData::destroy() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device->destroyBuffer(uniformBuffers[i]);
        uniformBuffers[i] = VK_NULL_HANDLE;
        device->freeMemory(uniformBuffersMemory[i]);
        uniformBuffersMemory[i] = VK_NULL_HANDLE;
    }
}

void vulkanSceneData::init(
    vulkanInstance* instance, 
    vk::DescriptorSetLayout descriptorSetLayout,
    vk::DescriptorPool descriptorPool
) {
    device = instance->getDevicePtr();
    initUniformBuffers(instance);
    initDescriptorSets(descriptorSetLayout, descriptorPool);
}

void vulkanSceneData::initUniformBuffers(vulkanInstance* instance) {
    vk::DeviceSize bufferSize = sizeof(sceneUniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        instance->initBuffer(
            bufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            uniformBuffers[i], uniformBuffersMemory[i]
        );

        uniformBuffersMapped[i] = device->mapMemory(uniformBuffersMemory[i], 0, bufferSize);
    }
}

void vulkanSceneData::updateSceneUniformBuffer(uint32_t currentFrame, vk::Extent2D extent, glm::mat4 viewMat) {
    sceneUniformBufferObject sbo{};
    sbo.view = viewMat;
    sbo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
    sbo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentFrame], &sbo, sizeof(sbo));
}

void vulkanSceneData::initDescriptorSets(
    vk::DescriptorSetLayout descriptorSetLayout,
    vk::DescriptorPool descriptorPool
) {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    auto allocInfo = vk::DescriptorSetAllocateInfo(
        descriptorPool,
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),    // descriptor set count
        layouts.data()                                  // descriptor set layouts
    );

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    try {
        descriptorSets = device->allocateDescriptorSets(allocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto sceneInfo = vk::DescriptorBufferInfo(
            uniformBuffers[i],
            0, sizeof(sceneUniformBufferObject)                 // offset, range
        );

        std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {
            vk::WriteDescriptorSet(
                descriptorSets[i], 0, 0,                   // dest set, binding, array element
                1, vk::DescriptorType::eUniformBuffer,          // descriptor count, type
                nullptr,
                &sceneInfo
            )
        };

        device->updateDescriptorSets(descriptorWrites, 0);
    }
}
