#ifndef VULKAN_DYNAMIC_UNIFORM_BUFFER_H_
#define VULKAN_DYNAMIC_UNIFORM_BUFFER_H_

#include "generalIncludes.h"
#include "modelDataIncludes.h"

#include "vulkanInstance.h"

class vulkanDynamicUniformBuffer {
private:
    std::vector<vk::Buffer> uniformBuffers;
    size_t dynamicAlignment;
    std::vector<vk::DeviceMemory> uniformBuffersMemory;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanDynamicUniformBuffer();

    std::vector<vk::Buffer> getUniformBuffers() { return uniformBuffers; }
    size_t getDynamicAlignment() { return dynamicAlignment; }

    void initUniformBuffers(vulkanInstance* instance);

    void updateModelUniformBuffer(glm::mat4 modelMat, uint32_t currentFrame, size_t objectNumber);

private:

};

#endif // VULKAN_DYNAMIC_UNIFORM_BUFFER_H_
