#ifndef VULKAN_OBJECT_H_
#define VULKAN_OBJECT_H_

#include "generalIncludes.h"
#include "modelDataIncludes.h"

#include "vulkanDynamicUniformBuffer.h"
#include "vulkanTextureData.h"
#include "vulkanSceneData.h"

#include <fstream>

static std::vector<char> readFile(const std::string& filename) {
    // ate -> reading from the end of the file
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    // create file big enough
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // seek back to start of file and start reading
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

class vulkanObject {
private:
    // Descriptor sets:
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorPool descriptorPool;
    std::vector<vk::DescriptorSet> descriptorSets;
    // Pipeline
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanObject();
    
    void setDevice(vk::Device _device) { device = _device; }

    vk::DescriptorPool getDescriptorPool() { return descriptorPool; }
    std::vector<vk::DescriptorSet> getDescriptorSets() { return descriptorSets; }
    vk::PipelineLayout getPipelineLayout() { return pipelineLayout; }
    vk::Pipeline getPipeline() { return graphicsPipeline; }

    void initPipeline(vk::Extent2D extent, vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSamples);
    void initDescriptorSetLayout();
    void initDescriptorPool();
    void initDescriptorSets(
        vulkanDynamicUniformBuffer* uniformBuffer,
        vulkanTextureData* textureData,
        vulkanSceneData* sceneData
    );

private:
    vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);
};

#endif // VULKAN_OBJECT_H_