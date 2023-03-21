#ifndef VULKAN_MODEL_DATA_H_
#define VULKAN_MODEL_DATA_H_

#include <chrono>
#include <unordered_map>

#include "generalIncludes.h"
#include "modelDataIncludes.h"

#include "vulkanInstance.h"

class vulkanModelData {
private:
    // Buffers:
    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;
    // Uniform buffers:
    std::vector<vk::Buffer> uniformBuffers;
    std::vector<vk::DeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    // Vertices + indices:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanModelData();

    void setDevice(vk::Device _device) { device = _device; }

    vk::Buffer getVertexBuffer() { return vertexBuffer; }
    vk::Buffer getIndexBuffer() { return indexBuffer; }
    std::vector<Vertex> getVertices() { return vertices; }
    std::vector<uint32_t> getIndices() { return indices; }
    std::vector<vk::Buffer> getUniformBuffers() { return uniformBuffers; }

    void loadModel(std::string modelPath);

    void initVertexBuffer(vulkanInstance* instance);
    void initIndexBuffer(vulkanInstance* instance);
    void initUniformBuffers(vulkanInstance* instance);

    void updateUniformBuffer(uint32_t currentImage, vk::Extent2D extent);

private:
    void copyBuffer(
        vk::Buffer src, vk::Buffer dst, vk::DeviceSize size,
        vulkanInstance* instance
    );
};

#endif // VULKAN_MODEL_DATA_H_