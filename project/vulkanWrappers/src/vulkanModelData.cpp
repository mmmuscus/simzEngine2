#include "../include/vulkanModelData.h"

vulkanModelData::~vulkanModelData() {
	device.destroyDescriptorPool(descriptorPool);
}

void vulkanModelData::init(
    vk::Device _device,
    vulkanMeshData* _meshData,
    vulkanTextureData* _textureData,
    vk::DescriptorSetLayout descriptorSetLayout
) {
    device = _device;
    meshData = _meshData;
    textureData = _textureData;
    initDescriptorPool();
    initDescriptorSets(descriptorSetLayout);
}

void vulkanModelData::initDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 2> poolSizes = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBufferDynamic,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        ),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eCombinedImageSampler,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        )
    };

    auto poolInfo = vk::DescriptorPoolCreateInfo(
        vk::DescriptorPoolCreateFlags(),
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),    // maxsets
        static_cast<uint32_t>(poolSizes.size()), poolSizes.data()
    );

    try {
        descriptorPool = device.createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vulkanModelData::initDescriptorSets(vk::DescriptorSetLayout descriptorSetLayout) {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    auto allocInfo = vk::DescriptorSetAllocateInfo(
        descriptorPool,
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),    // descriptor set count
        layouts.data()                                  // descriptor set layouts
    );

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    try {
        descriptorSets = device.allocateDescriptorSets(allocInfo);
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

        device.updateDescriptorSets(descriptorWrites, 0);
    }
}
