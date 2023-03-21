#ifndef VULKAN_TEXTURE_DATA_H_
#define VULKAN_TEXTURE_DATA_H_

#include "generalIncludes.h"

#include "vulkanInstance.h"

class vulkanTextureData {
private:
    // Textures:
    uint32_t mipLevels;
    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView textureImageView;
    vk::Sampler textureSampler;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanTextureData();

    void setDevice(vk::Device _device) { device = _device; }

    vk::ImageView getImageView() { return textureImageView; }
    vk::Sampler getSampler() { return textureSampler; }

    void initTextureImage(std::string texturePath, vulkanInstance* instance);
    void initTextureImageView(vulkanInstance* instance);
    void initTextureSampler(vk::PhysicalDevice physicalDevice);

private:
    void transitionImageLayout(
        vk::Image image, vk::Format format,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        uint32_t mipLevels,
        vulkanInstance* instance
    );
    void copyBufferToImage(
        vk::Buffer buffer, vk::Image image,
        uint32_t width, uint32_t height,
        vulkanInstance* instance
    );
    
    void generateMipmaps(
        vk::Image image, vk::Format imageFormat,
        int32_t texWidth, int32_t texHeight,
        uint32_t mipLevels,
        vulkanInstance* instance
    );
};

#endif // VULKAN_TEXTURE_DATA_H_