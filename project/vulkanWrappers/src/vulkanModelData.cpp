#include "../include/vulkanModelData.h"

void vulkanModelData::init(
    vulkanInstance* instance,
    meshDataManager* meshManager, std::string meshPath, vulkanDynamicUniformBuffer* uniformBuffer,
    textureDataManager* textureManager, std::string texturePath, vulkanTextureSampler* textureSampler,
    vk::DescriptorSetLayout descriptorSetLayout, vk::DescriptorPool descriptorPool
) {
    device = instance->getDevice();
    meshData = meshManager->addMeshData(meshPath, instance, uniformBuffer);
    textureData = textureManager->addTextureData(texturePath, instance, textureSampler);
    initDescriptorSets(descriptorSetLayout, descriptorPool);
}

void vulkanModelData::init(
    vk::Device* _device,
    vulkanMeshData* _meshData,
    vulkanTextureData* _textureData,
    vk::DescriptorSetLayout descriptorSetLayout,
    vk::DescriptorPool descriptorPool
) {
    device = _device;
    meshData = _meshData;
    textureData = _textureData;
    initDescriptorSets(descriptorSetLayout, descriptorPool);
}

void vulkanModelData::initDescriptorSets(
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
        auto modelInfo = vk::DescriptorBufferInfo(
            meshData->getUniformBuffer()->getUniformBuffers()[i],
            0, sizeof(modelUniformBufferObject)                 // offset, range
        );

        auto imageInfo = vk::DescriptorImageInfo(
            textureData->getSampler()->get(),
            textureData->getImageView(),
            vk::ImageLayout::eShaderReadOnlyOptimal
        );

        std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {
            vk::WriteDescriptorSet(
                descriptorSets[i], 0, 0,                        // dest set, binding, array element
                1, vk::DescriptorType::eUniformBufferDynamic,   // descriptor count, type
                nullptr,                                        // image info
                &modelInfo
            ),
            vk::WriteDescriptorSet(
                descriptorSets[i], 1, 0,                        // dest set, binding, array element
                1, vk::DescriptorType::eCombinedImageSampler,   // descriptor count, type
                &imageInfo
            )
        };

        device->updateDescriptorSets(descriptorWrites, 0);
    }
}
