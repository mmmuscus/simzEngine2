#include "../include/vulkanSceneData.h"

vulkanSceneData::~vulkanSceneData() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroyBuffer(uniformBuffers[i]);
        device.freeMemory(uniformBuffersMemory[i]);
    }
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

        uniformBuffersMapped[i] = device.mapMemory(uniformBuffersMemory[i], 0, bufferSize);
    }
}

void vulkanSceneData::updateSceneUniformBuffer(uint32_t currentFrame, vk::Extent2D extent, glm::mat4 viewMat) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime)
        .count();

    sceneUniformBufferObject sbo{};
    sbo.view = viewMat;
    sbo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
    sbo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentFrame], &sbo, sizeof(sbo));
}