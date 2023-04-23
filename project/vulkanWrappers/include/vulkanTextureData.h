#ifndef VULKAN_TEXTURE_DATA_H_
#define VULKAN_TEXTURE_DATA_H_

#include <filesystem>

#include "generalIncludes.h"

#include "vulkanInstance.h"
#include "vulkanTextureSampler.h"

#include <stb/stb_image.h>

class vulkanTextureData {
private:
    // Name:
    std::string name;

    // Textures:
    uint32_t mipLevels;
    vk::Image image;
    vk::DeviceMemory imageMemory;
    vk::ImageView imageView;

    // Sampler
    vulkanTextureSampler* sampler;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanTextureData();

    void setDevice(vk::Device _device) { device = _device; }
    void setSampler(vulkanTextureSampler* _sampler) { sampler = _sampler; }

    std::string getName() { return name; }
    vk::ImageView getImageView() { return imageView; }
    vulkanTextureSampler* getSampler() { return sampler; }

    void init(
        std::string texturePath,
        vulkanInstance* instance,
        vulkanTextureSampler* _sampler
    );

private:
    void initTextureImage(std::string texturePath, vulkanInstance* instance);
    void initTextureImageView(vulkanInstance* instance);

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