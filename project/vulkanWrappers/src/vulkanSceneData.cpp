#include "../include/vulkanSceneData.h"

vulkanSceneData::~vulkanSceneData() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroyBuffer(sceneBuffers[i]);
        device.freeMemory(sceneBuffersMemory[i]);
    }
}

void vulkanSceneData::initSceneBuffers(vulkanInstance* instance) {
    vk::DeviceSize bufferSize = sizeof(sceneUniformBufferObject);

    sceneBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    sceneBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    sceneBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        instance->initBuffer(
            bufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            sceneBuffers[i], sceneBuffersMemory[i]
        );

        sceneBuffersMapped[i] = device.mapMemory(sceneBuffersMemory[i], 0, bufferSize);
    }
}

void vulkanSceneData::updateSceneBuffer(uint32_t currentImage, vk::Extent2D extent, glm::mat4 viewMat) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime)
        .count();

    sceneUniformBufferObject sbo{};
    // sbo.view = viewMat;
    sbo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    sbo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
    sbo.proj[1][1] *= -1;

    memcpy(sceneBuffersMapped[currentImage], &sbo, sizeof(sbo));
}