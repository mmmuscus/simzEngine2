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

    printf("%d\n", buffer.size());

    return buffer;
}

class vkPipelineWrapper {
private:


    // The following resources are not being maintained by the class
    VkDevice* device;

public:
    vkPipelineWrapper() : device(nullptr) {}

    void initGraphicsPipeline();

private:
    VkShaderModule createShaderModule(const std::vector<char>& code);

};

#endif // VK_PIPELINE_WRAPPER_H_