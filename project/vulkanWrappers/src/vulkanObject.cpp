#include "../include/vulkanObject.h"

vulkanObject::~vulkanObject() {
    device.destroyPipeline(graphicsPipeline);
    device.destroyPipelineLayout(pipelineLayout);
}

void vulkanObject::initPipeline(vk::Extent2D extent, vk::RenderPass renderPass) {
    auto vertShaderCode = readFile("shaders/vertexShaders/vert.spv");
    auto fragShaderCode = readFile("shaders/fragmentShaders/frag.spv");

    auto vertShaderModule = createShaderModule(vertShaderCode);
    auto fragShaderModule = createShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo shaderStages[] = {
        {
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eVertex,
            *vertShaderModule,
            "main"
        },
        {
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eFragment,
            *fragShaderModule,
            "main"
        }
    };

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    auto vertexInputInfo = vk::PipelineVertexInputStateCreateInfo(
        vk::PipelineVertexInputStateCreateFlags(),
        1, &bindingDescription,
        static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data()
    );

    auto inputAssembly = vk::PipelineInputAssemblyStateCreateInfo(
        vk::PipelineInputAssemblyStateCreateFlags(),
        vk::PrimitiveTopology::eTriangleList,
        VK_FALSE
    );

    auto viewportState = vk::PipelineViewportStateCreateInfo(
        vk::PipelineViewportStateCreateFlags(),
        1, nullptr,                     // viewports
        1, nullptr                      // scissors
    );

    auto rasterizer = vk::PipelineRasterizationStateCreateInfo(
        vk::PipelineRasterizationStateCreateFlags(),
        VK_FALSE,
        VK_FALSE,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack,
        vk::FrontFace::eClockwise,
        VK_FALSE, 0.0f, 0.0f, 0.0f,     // depth bias
        1.0f
    );

    auto multisampling = vk::PipelineMultisampleStateCreateInfo(
        vk::PipelineMultisampleStateCreateFlags(),
        vk::SampleCountFlagBits::e1,
        VK_FALSE
    );

    auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState(
        VK_FALSE,
        vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,      // color blend
        vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,      // alpha blend
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    );

    auto colorBlending = vk::PipelineColorBlendStateCreateInfo(
        vk::PipelineColorBlendStateCreateFlags(),
        VK_FALSE, vk::LogicOp::eCopy,   // logic operation
        1, &colorBlendAttachment,
        { 0.0f, 0.0f, 0.0f, 0.0f }      // blend constants
    );

    std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
    };

    auto dynamicState = vk::PipelineDynamicStateCreateInfo(
        vk::PipelineDynamicStateCreateFlags(),
        static_cast<uint32_t>(dynamicStates.size()),
        dynamicStates.data()
    );

    auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo(
        vk::PipelineLayoutCreateFlags(),
        0, nullptr,                     // set layouts
        0, nullptr                      // push constant range
    );

    try {
        pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    auto pipelineInfo = vk::GraphicsPipelineCreateInfo(
        vk::PipelineCreateFlags(),
        2, shaderStages,                // (shader) stages
        &vertexInputInfo,
        &inputAssembly,
        nullptr,                        // tesselation state
        &viewportState,
        &rasterizer,
        &multisampling,
        nullptr,                        // depth stencil state
        &colorBlending,
        &dynamicState,
        pipelineLayout,
        renderPass,
        0,                              // subpass
        nullptr                         // base pipeline handle
    );

    try {
        vk::Result result;
        std::tie( result, graphicsPipeline ) = device.createGraphicsPipeline(nullptr, pipelineInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

vk::UniqueShaderModule vulkanObject::createShaderModule(const std::vector<char>& code) {
    try {
        return device.createShaderModuleUnique({
            vk::ShaderModuleCreateFlags(),
            code.size(),
            reinterpret_cast<const uint32_t*>(code.data())
            });
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create shader module!");
    }
}
