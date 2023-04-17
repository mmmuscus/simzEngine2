#include "../include/vulkanObject.h"

vulkanObject::~vulkanObject() {
    // Old descriptor sets:
    device.destroyDescriptorPool(descriptorPool);
    device.destroyDescriptorSetLayout(descriptorSetLayout);
    // New descriptor sets:
    device.destroyDescriptorPool(sceneDescriptorPool);
    device.destroyDescriptorSetLayout(sceneDescriptorSetLayout);
    device.destroyDescriptorPool(modelDescriptorPool);
    device.destroyDescriptorSetLayout(modelDescriptorSetLayout);

    device.destroyPipeline(graphicsPipeline);
    device.destroyPipelineLayout(pipelineLayout);
}

void vulkanObject::initPipeline(vk::Extent2D extent, vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSamples) {
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

    // vk::DescriptorSetLayout descriptorSetLayouts[] = { modelDescriptorSetLayout, sceneDescriptorSetLayout };
    auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo(
        vk::PipelineLayoutCreateFlags(),
        1, &descriptorSetLayout,        // set layouts
        //2, descriptorSetLayouts,        // set layouts
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

void vulkanObject::initSceneDescriptorSetLayout() {
    auto sceneLayoutBinding = vk::DescriptorSetLayoutBinding(
        2,                                              // binding
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
        vk::DescriptorPoolCreateFlags(),
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),    // maxsets
        static_cast<uint32_t>(poolSizes.size()), poolSizes.data()
    );

    try {
        sceneDescriptorPool = device.createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vulkanObject::initSceneDescriptorSets(vulkanSceneData* sceneData) {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, sceneDescriptorSetLayout);

    auto allocInfo = vk::DescriptorSetAllocateInfo(
        sceneDescriptorPool,
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),    // descriptor set count
        layouts.data()                                  // descriptor set layouts
    );

    sceneDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    try {
        sceneDescriptorSets = device.allocateDescriptorSets(allocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto sceneInfo = vk::DescriptorBufferInfo(
            sceneData->getUniformBuffers()[i],
            0, sizeof(sceneUniformBufferObject)                 // offset, range
        );

        std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {
            vk::WriteDescriptorSet(
                sceneDescriptorSets[i], 2, 0,                        // dest set, binding, array element
                1, vk::DescriptorType::eUniformBuffer,          // descriptor count, type
                nullptr,
                &sceneInfo
            )
        };

        device.updateDescriptorSets(descriptorWrites, 0);
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
        1,                                              // bining
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
        vk::DescriptorPoolCreateFlags(),
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),    // maxsets
        static_cast<uint32_t>(poolSizes.size()), poolSizes.data()
    );

    try {
        modelDescriptorPool = device.createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vulkanObject::initModelDescriptorSets(
    vulkanDynamicUniformBuffer* uniformBuffer,
    vulkanTextureData* textureData
) {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, modelDescriptorSetLayout);

    auto allocInfo = vk::DescriptorSetAllocateInfo(
        modelDescriptorPool,
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),    // descriptor set count
        layouts.data()                                  // descriptor set layouts
    );

    modelDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    try {
        modelDescriptorSets = device.allocateDescriptorSets(allocInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto modelInfo = vk::DescriptorBufferInfo(
            uniformBuffer->getUniformBuffers()[i],
            0, sizeof(modelUniformBufferObject)                 // offset, range
        );

        auto imageInfo = vk::DescriptorImageInfo(
            textureData->getSampler(),
            textureData->getImageView(),
            vk::ImageLayout::eShaderReadOnlyOptimal
        );

        std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {
            vk::WriteDescriptorSet(
                modelDescriptorSets[i], 0, 0,                   // dest set, binding, array element
                1, vk::DescriptorType::eUniformBufferDynamic,   // descriptor count, type
                nullptr,                                        // image info
                &modelInfo
            ),
            vk::WriteDescriptorSet(
                modelDescriptorSets[i], 1, 0,                    // dest set, binding, array element
                1, vk::DescriptorType::eCombinedImageSampler,   // descriptor count, type
                &imageInfo
            )
        };

        device.updateDescriptorSets(descriptorWrites, 0);
    }
}

void vulkanObject::initDescriptorSetLayout() {
    auto modelLayoutBinding = vk::DescriptorSetLayoutBinding(
        0,                                              // binding
        vk::DescriptorType::eUniformBufferDynamic, 1,   // descriptor type, count
        vk::ShaderStageFlagBits::eVertex,
        nullptr
    );

    auto samplerLayoutBinding = vk::DescriptorSetLayoutBinding(
        1,                                              // bining
        vk::DescriptorType::eCombinedImageSampler, 1,   // decriptor type, count
        vk::ShaderStageFlagBits::eFragment,
        nullptr
    );

    auto sceneLayoutBinding = vk::DescriptorSetLayoutBinding(
        2,                                              // binding
        vk::DescriptorType::eUniformBuffer, 1,          // descriptor type, count
        vk::ShaderStageFlagBits::eVertex,
        nullptr
    );

    std::array<vk::DescriptorSetLayoutBinding, 3> bindings = { 
        modelLayoutBinding, 
        samplerLayoutBinding,
        sceneLayoutBinding
    };

    auto layoutInfo = vk::DescriptorSetLayoutCreateInfo(
        vk::DescriptorSetLayoutCreateFlags(),
        static_cast<uint32_t>(bindings.size()), bindings.data()
    );

    try {
        descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void vulkanObject::initDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 3> poolSizes = {
        vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBufferDynamic,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        ),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eCombinedImageSampler,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        ),
        vk::DescriptorPoolSize(
            vk::DescriptorType::eUniformBuffer,
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
    } catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vulkanObject::initDescriptorSets(
    vulkanDynamicUniformBuffer* uniformBuffer, 
    vulkanTextureData* textureData,
    vulkanSceneData* sceneData
) {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    
    auto allocInfo = vk::DescriptorSetAllocateInfo(
        descriptorPool,
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),    // descriptor set count
        layouts.data()                                  // descriptor set layouts
    );

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    try {
        descriptorSets = device.allocateDescriptorSets(allocInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error("failed to create descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto modelInfo = vk::DescriptorBufferInfo(
            uniformBuffer->getUniformBuffers()[i],
            0, sizeof(modelUniformBufferObject)                 // offset, range
        );

        auto imageInfo = vk::DescriptorImageInfo(
            textureData->getSampler(),
            textureData->getImageView(),
            vk::ImageLayout::eShaderReadOnlyOptimal
        );

        auto sceneInfo = vk::DescriptorBufferInfo(
            sceneData->getUniformBuffers()[i],
            0, sizeof(sceneUniformBufferObject)                 // offset, range
        );

        std::array<vk::WriteDescriptorSet, 3> descriptorWrites = {
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
            ),
            vk::WriteDescriptorSet(
                descriptorSets[i], 2, 0,                        // dest set, binding, array element
                1, vk::DescriptorType::eUniformBuffer,          // descriptor count, type
                nullptr,
                &sceneInfo
            )
        };

        device.updateDescriptorSets(descriptorWrites, 0);
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