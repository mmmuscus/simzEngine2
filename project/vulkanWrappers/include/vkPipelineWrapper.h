#ifndef VK_PIPELINE_WRAPPER_H_
#define VK_PIPELINE_WRAPPER_H_

#include "generalIncludes.h"

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

class vkPipelineWrapper {
private:
    VkRenderPass* renderPass;
    VkPipelineLayout* pipelineLayout;
    VkPipeline* graphicsPipeline;

    // The following resources are not being maintained by the class
    VkDevice* device;

public:
    vkPipelineWrapper() : 
        renderPass(nullptr), 
        pipelineLayout(nullptr), 
        graphicsPipeline(nullptr), 
        device(nullptr) {}
    ~vkPipelineWrapper();

    VkRenderPass* getRenderPass() { return renderPass; }
    VkPipeline* getGraphicsPipeline() { return graphicsPipeline; }

    void init(VkDevice* _device, VkFormat* swapChainImageFormat);

private:
    void initRenderPass(VkFormat* swapChainImageFormat);
    void initGraphicsPipeline();

    VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif // VK_PIPELINE_WRAPPER_H_