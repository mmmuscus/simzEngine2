#ifndef VULKAN_MODEL_DATA_H_
#define VULKAN_MODEL_DATA_H_

#include <chrono>

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
    // TODO: move to seperate TextureData class
    // Texture:
    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView textureImageView;
    vk::Sampler textureSampler;

    // Vertices + indices:
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
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
    std::vector<vk::Buffer> getUniformBuffers() { return uniformBuffers; }
    vk::ImageView getImageView() { return textureImageView; }
    vk::Sampler getSampler() { return textureSampler; }

    void initVertexBuffer(vulkanInstance* instance);
    void initIndexBuffer(vulkanInstance* instance);
    void initUniformBuffers(vulkanInstance* instance);
    void initTextureImage(vulkanInstance* instance);
    void initTextureImageView(vulkanInstance* instance);
    void initTextureSampler(vk::PhysicalDevice physicalDevice);

    void updateUniformBuffer(uint32_t currentImage, vk::Extent2D extent);

private:
    void copyBuffer(
        vk::Buffer src, vk::Buffer dst, vk::DeviceSize size,
        vulkanInstance* instance
    );

    void transitionImageLayout(
        vk::Image image, vk::Format format,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vulkanInstance* instance
    );
    void copyBufferToImage(
        vk::Buffer buffer, vk::Image image,
        uint32_t width, uint32_t height,
        vulkanInstance* instance
    );
};

#endif // VULKAN_MODEL_DATA_H_