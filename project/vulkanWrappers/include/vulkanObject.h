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
    vk::DescriptorSetLayout sceneDescriptorSetLayout;
    vk::DescriptorPool sceneDescriptorPool;
    std::vector<vk::DescriptorSet> sceneDescriptorSets;
    vk::DescriptorSetLayout modelDescriptorSetLayout;
    vk::DescriptorPool modelDescriptorPool;
    std::vector<vk::DescriptorSet> modelDescriptorSets;
    // Pipeline
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanObject();
    
    void setDevice(vk::Device _device) { device = _device; }

    // Descriptor sets:
    vk::DescriptorPool getSceneDescriptorPool() { return sceneDescriptorPool; }
    std::vector<vk::DescriptorSet> getSceneDescriptorSets() { return sceneDescriptorSets; }
    vk::DescriptorPool getModelDescriptorPool() { return modelDescriptorPool; }
    std::vector<vk::DescriptorSet> getModelDescriptorSets() { return modelDescriptorSets; }
    vk::PipelineLayout getPipelineLayout() { return pipelineLayout; }
    vk::Pipeline getPipeline() { return graphicsPipeline; }

    void initPipeline(vk::Extent2D extent, vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSamples);
    // Descriptor sets:
    void initSceneDescriptorSetLayout();
    void initSceneDescriptorPool();
    void initSceneDescriptorSets(vulkanSceneData* sceneData);
    void initModelDescriptorSetLayout();
    void initModelDescriptorPool();
    void initModelDescriptorSets(
        vulkanDynamicUniformBuffer* uniformBuffer,
        vulkanTextureData* textureData
    );

private:
    vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);
};

#endif // VULKAN_OBJECT_H_