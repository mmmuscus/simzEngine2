#ifndef VULKAN_MODEL_DATA_H_
#define VULKAN_MODEL_DATA_H_

#include "generalIncludes.h"

#include "vulkanInstance.h"

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return vk::VertexInputBindingDescription(
            0,
            sizeof(Vertex),
            vk::VertexInputRate::eVertex
        );
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
        auto posInputAttributeDescription = vk::VertexInputAttributeDescription(
            0, 0,                       // location, binding
            vk::Format::eR32G32Sfloat,
            offsetof(Vertex, pos)
        );

        auto colorInputAttributeDesciption = vk::VertexInputAttributeDescription(
            1, 0,                       // location, binding
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, color)
        );

        return std::array<vk::VertexInputAttributeDescription, 2> {
            posInputAttributeDescription,
                colorInputAttributeDesciption
        };
    }
};

class vulkanModelData {
private:
    // Buffers:
    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;

    // Vertices + indices:
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanModelData();

    void setDevice(vk::Device _device) { device = _device; }

    vk::Buffer getVertexBuffer() { return vertexBuffer; }
    vk::Buffer getIndexBuffer() { return indexBuffer; }
    std::vector<Vertex> getVertices() { return vertices; }
    std::vector<uint16_t> getIndices() { return indices; }

    void initVertexBuffer(vulkanInstance* instance, vk::CommandPool commandPool);
    void initIndexBuffer(vulkanInstance* instance, vk::CommandPool commandPool);

private:
    void initBuffer(
        vk::DeviceSize size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        vk::PhysicalDevice physicalDevice,
        vk::Buffer& buffer,
        vk::DeviceMemory& bufferMemory
    );
    void copyBuffer(
        vk::Buffer src, vk::Buffer dst, vk::DeviceSize size,
        vk::CommandPool commandPool, vk::Queue graphicsQueue
    );

    uint32_t findMemoryType(
        uint32_t typeFilter,
        vk::MemoryPropertyFlags properties,
        vk::PhysicalDevice physicalDevice
    );
};

#endif // VULKAN_MODEL_DATA_H_