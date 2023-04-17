#ifndef VULKAN_MODEL_DATA_H_
#define VULKAN_MODEL_DATA_H_

#include <chrono>
#include <unordered_map>

#include "generalIncludes.h"
#include "modelDataIncludes.h"

#include "vulkanInstance.h"
#include "vulkanDynamicUniformBuffer.h"

class vulkanModelData {
private:
    // Buffers:
    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;
    // Uniform buffers:
    vulkanDynamicUniformBuffer* uniformBuffer;

    // Vertices + indices:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanModelData();

    void setDevice(vk::Device _device) { device = _device; }
    void setDynamicUniformBuffer(vulkanDynamicUniformBuffer* _uniformBuffer) { uniformBuffer = _uniformBuffer; }

    vk::Buffer getVertexBuffer() { return vertexBuffer; }
    vk::Buffer getIndexBuffer() { return indexBuffer; }
    vulkanDynamicUniformBuffer* getUniformBuffer() { return uniformBuffer; }
    std::vector<Vertex> getVertices() { return vertices; }
    std::vector<uint32_t> getIndices() { return indices; }

    void init(std::string modelPath, vulkanInstance* instance);

private:
    void loadModel(std::string modelPath);
    void initVertexBuffer(vulkanInstance* instance);
    void initIndexBuffer(vulkanInstance* instance);

    void copyBuffer(
        vk::Buffer src, vk::Buffer dst, vk::DeviceSize size,
        vulkanInstance* instance
    );
};

#endif // VULKAN_MODEL_DATA_H_