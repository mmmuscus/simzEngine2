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
    // Descriptor set layouts:
    vk::DescriptorSetLayout sceneDescriptorSetLayout;
    vk::DescriptorPool modelDescriptorPool;
    vk::DescriptorSetLayout modelDescriptorSetLayout;
    // Pipeline
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanObject();
    
    void setDevice(vk::Device _device) { device = _device; }

    // Descriptor set layouts:
    vk::DescriptorSetLayout getSceneDescriptorSetLayout() { return sceneDescriptorSetLayout; }
    vk::DescriptorPool getModelDescriptorPool() { return modelDescriptorPool; }
    vk::DescriptorSetLayout getModelDescriptorSetLayout() { return modelDescriptorSetLayout; }
    vk::PipelineLayout getPipelineLayout() { return pipelineLayout; }
    vk::Pipeline getPipeline() { return graphicsPipeline; }
    vk::Device getDevice() { return device; }

    void initPipeline(
        vk::Extent2D extent, vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSamples,
        std::string vertexShaderPath, std::string fragmentShaderPath
    );
    // Descriptor set layouts:
    void initDescriptors();

private:
    void initSceneDescriptorSetLayout();
    void initModelDescriptorSetLayout();
    void initModelDescriptorPool();

    vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);
};

#endif // VULKAN_OBJECT_H_