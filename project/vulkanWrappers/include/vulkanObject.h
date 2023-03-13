#ifndef VULKAN_OBJECT_H_
#define VULKAN_OBJECT_H_

#include "generalIncludes.h"

#include <fstream>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return vk::VertexInputBindingDescription(
            0,
            sizeof(Vertex),
            vk::VertexInputRate::eVertex
        );
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
        auto posInputAttributeDescription = vk::VertexInputAttributeDescription(
            0, 0,                       // location, binding
            vk::Format::eR32G32Sfloat,
            offsetof(Vertex, pos)
        );

        auto colorInputAttributeDesciption = vk::VertexInputAttributeDescription(
            1, 0,                       // location, binding
            vk::Format::eR32G32Sfloat,
            offsetof(Vertex, color)
        );

        return std::array<vk::VertexInputAttributeDescription, 2> {
            posInputAttributeDescription,
            colorInputAttributeDesciption
        };
    }
};

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
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

    // Not maintained by the class:
    vk::Device device;

public:
    ~vulkanObject();
    
    void setDevice(vk::Device _device) { device = _device; }

    vk::Pipeline getPipeline() { return graphicsPipeline; }

    void initPipeline(vk::Extent2D extent, vk::RenderPass renderPass);

private:
    vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);
};

#endif // VULKAN_OBJECT_H_