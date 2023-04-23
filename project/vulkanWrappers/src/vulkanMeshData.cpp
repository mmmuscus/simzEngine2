#include "../include/vulkanMeshData.h"

vulkanMeshData::~vulkanMeshData() {
    device.destroyBuffer(indexBuffer);
    device.freeMemory(indexBufferMemory);

    device.destroyBuffer(vertexBuffer);
    device.freeMemory(vertexBufferMemory);
}

void vulkanMeshData::init(
    std::string meshPath, 
    vulkanInstance* instance, 
    vulkanDynamicUniformBuffer* _uniformBuffer
) {
    device = instance->getDevice();
    uniformBuffer = _uniformBuffer;
    loadMesh(meshPath);
    initVertexBuffer(instance);
    initIndexBuffer(instance);
}

void vulkanMeshData::loadMesh(std::string meshPath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!std::filesystem::exists(meshPath.c_str())) {
        printf("%s: Invalid mesh path!\n", meshPath.c_str());
    }

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    name = meshPath.substr(meshPath.find_last_of("/\\") + 1);
    std::string::size_type lastDot(name.find_last_of('.'));
    name = name.substr(0, lastDot);
    printf("%s\n", name.c_str());

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void vulkanMeshData::initVertexBuffer(vulkanInstance* instance) {
    vk::PhysicalDevice physicalDevice = instance->getPhysicalDevice();
    vk::Queue graphicsQueue = instance->getGraphicsQueue();

    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    instance->initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory
    );

    void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    device.unmapMemory(stagingBufferMemory);

    instance->initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        vertexBuffer, vertexBufferMemory
    );

    copyBuffer(
        stagingBuffer, vertexBuffer, bufferSize,
        instance
    );

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void vulkanMeshData::initIndexBuffer(vulkanInstance* instance) {
    vk::PhysicalDevice physicalDevice = instance->getPhysicalDevice();
    vk::Queue graphicsQueue = instance->getGraphicsQueue();

    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    instance->initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory
    );

    void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), (size_t)bufferSize);
    device.unmapMemory(stagingBufferMemory);

    instance->initBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        indexBuffer, indexBufferMemory
    );

    copyBuffer(
        stagingBuffer, indexBuffer, bufferSize,
        instance
    );

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void vulkanMeshData::copyBuffer(
    vk::Buffer src, vk::Buffer dst, vk::DeviceSize size,
    vulkanInstance* instance
) {
    vk::CommandBuffer commandBuffer = instance->beginSingleTimeCommands();

    auto copyRegion = vk::BufferCopy(0, 0, size);   // src, dst offset
    commandBuffer.copyBuffer(src, dst, copyRegion);

    instance->endSingleTimeCommands(commandBuffer);
}