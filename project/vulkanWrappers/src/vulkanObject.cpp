#include "../include/vulkanObject.h"

vulkanObject::~vulkanObject() {
    destroy();
}

void vulkanObject::destroy() {
    // Descriptor Pools:
    device.destroyDescriptorPool(modelDescriptorPool);
    modelDescriptorPool = VK_NULL_HANDLE;
    device.destroyDescriptorPool(sceneDescriptorPool);
    sceneDescriptorPool = VK_NULL_HANDLE;

    // Descriptor set Layouts:
    device.destroyDescriptorSetLayout(sceneDescriptorSetLayout);
    sceneDescriptorSetLayout = VK_NULL_HANDLE;
    device.destroyDescriptorSetLayout(modelDescriptorSetLayout);
    modelDescriptorSetLayout = VK_NULL_HANDLE;

    // Pipeline and Pipeline layout
    device.destroyPipeline(graphicsPipeline);
    graphicsPipeline = VK_NULL_HANDLE;
    device.destroyPipelineLayout(pipelineLayout);
    pipelineLayout = VK_NULL_HANDLE;
}

void vulkanObject::init(
    std::string _name, vk::Device _device,
    vk::Extent2D extent, vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSamples,
    std::string vertexShaderPath, std::string fragmentShaderPath
) {
    name = _name;
    device = _device;
    initDescriptors();
    initPipeline(
        extent, renderPass, msaaSamples,
        vertexShaderPath, fragmentShaderPath
    );
}

void vulkanObject::initPipeline(
    vk::Extent2D extent, vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSamples,
    std::string vertexShaderPath, std::string fragmentShaderPath
) {
    auto vertShaderCode = readFile(vertexShaderPath);
    auto fragShaderCode = readFile(fragmentShaderPath);

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
        vk::FrontFace::eCounterClockwise,
        VK_FALSE, 0.0f, 0.0f, 0.0f,     // depth bias
        1.0f
    );

    auto multisampling = vk::PipelineMultisampleStateCreateInfo(
        vk::PipelineMultisampleStateCreateFlags(),
        msaaSamples,
        VK_TRUE,                        // sample shading enable
        .2f                             // min sample shading
    );

    auto depthStencil = vk::PipelineDepthStencilStateCreateInfo(
        vk::PipelineDepthStencilStateCreateFlags(),
        VK_TRUE, VK_TRUE,               // depth test, write enable
        vk::CompareOp::eLess,
        VK_FALSE,                       // depth bounds tet enable
        VK_FALSE,                       // stencil test enable
        vk::StencilOpState(), vk::StencilOpState(),
        0.0f, 1.0f                      // min, max depth
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

    vk::DescriptorSetLayout descriptorSetLayouts[] = { sceneDescriptorSetLayout, modelDescriptorSetLayout };
    auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo(
        vk::PipelineLayoutCreateFlags(),
        //1, &descriptorSetLayout,        // set layouts
        2, descriptorSetLayouts,        // set layouts
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
        &depthStencil,
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

void vulkanObject::initDescriptors() {
    initSceneDescriptorSetLayout();
    initSceneDescriptorPool();
    initModelDescriptorSetLayout();
    initModelDescriptorPool();
}

void vulkanObject::initSceneDescriptorSetLayout() {
    auto sceneLayoutBinding = vk::DescriptorSetLayoutBinding(
        0,                                              // binding
        vk::DescriptorType::eUniformBuffer, 1,          // descriptor type, count
        vk::ShaderStageFlagBits::eVertex,
        nullptr
    );

    std::array<vk::DescriptorSetLayoutBinding, 1> bindings = { sceneLayoutBinding };

    auto layoutInfo = vk::DescriptorSetLayoutCreateInfo(
        vk::DescriptorSetLayoutCreateFlags(),
        static_cast<uint32_t>(bindings.size()), bindings.data()
    );

    try {
        sceneDescriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void vulkanObject::initSceneDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBuffer,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        )
    };

    auto poolInfo = vk::DescriptorPoolCreateInfo(
        vk::DescriptorPoolCreateFlags(), MAX_DESCRIPTORPOOL_SETS,
        static_cast<uint32_t>(poolSizes.size()), poolSizes.data()
    );

    try {
        sceneDescriptorPool = device.createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vulkanObject::initModelDescriptorSetLayout() {
    auto modelLayoutBinding = vk::DescriptorSetLayoutBinding(
        0,                                              // binding
        vk::DescriptorType::eUniformBufferDynamic, 1,   // descriptor type, count
        vk::ShaderStageFlagBits::eVertex,
        nullptr
    );

    auto samplerLayoutBinding = vk::DescriptorSetLayoutBinding(
        1,                                              // binding
        vk::DescriptorType::eCombinedImageSampler, 1,   // decriptor type, count
        vk::ShaderStageFlagBits::eFragment,
        nullptr
    );

    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {
        modelLayoutBinding,
        samplerLayoutBinding
    };

    auto layoutInfo = vk::DescriptorSetLayoutCreateInfo(
        vk::DescriptorSetLayoutCreateFlags(),
        static_cast<uint32_t>(bindings.size()), bindings.data()
    );

    try {
        modelDescriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void vulkanObject::initModelDescriptorPool() {
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
        vk::DescriptorPoolCreateFlags(), MAX_DESCRIPTORPOOL_SETS,
        static_cast<uint32_t>(poolSizes.size()), poolSizes.data()
    );

    try {
        modelDescriptorPool = device.createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor pool!");
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