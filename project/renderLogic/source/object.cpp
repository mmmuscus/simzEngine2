#include "../include/object.h"

object::~object() {
    //device.destroyDescriptorPool(descriptorPool);
}

void object::initDescriptors() {
    device = vkObject->getDevice();
    initDescriptorPool();
    initDescriptorSets();
}

void object::initDescriptorPool() {
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

void object::initDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, vkObject->getModelDescriptorSetLayout());

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
            modelData->getUniformBuffer()->getUniformBuffers()[i],
            0, sizeof(modelUniformBufferObject)                 // offset, range
        );

        auto imageInfo = vk::DescriptorImageInfo(
            textureData->getSampler(),
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

void object::updateTranslationVectors() {
	sceneTimer->updateTime();

	pos = pos;
	rotation.x = sceneTimer->getTime() * glm::radians(90.0f);
	scale = scale;
}

void object::updateModelTranslation(uint32_t currentFrame) {
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, pos);
	trans = glm::rotate(trans, rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, scale);

	modelData->getUniformBuffer()->updateModelUniformBuffer(trans, currentFrame, objectNumber);
}