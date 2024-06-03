#ifndef MODEL_DATA_INCLUDES_H_
#define MODEL_DATA_INCLUDES_H_

#include "generalIncludes.h"

#include <tinyobjloader/tiny_obj_loader.h>

struct modelUniformBufferObject {
    alignas(16) glm::mat4 model;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return vk::VertexInputBindingDescription(
            0,
            sizeof(Vertex),
            vk::VertexInputRate::eVertex
        );
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        auto posInputAttributeDescription = vk::VertexInputAttributeDescription(
            0, 0,                       // location, binding
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, pos)
        );

        auto colorInputAttributeDesciption = vk::VertexInputAttributeDescription(
            1, 0,                       // location, binding
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, color)
        );

        auto textureInputAttributeDescription = vk::VertexInputAttributeDescription(
            2, 0,                       // loaction, binding
            vk::Format::eR32G32Sfloat,
            offsetof(Vertex, texCoord)
        );

        return std::array<vk::VertexInputAttributeDescription, 3> {
            posInputAttributeDescription,
            colorInputAttributeDesciption,
            textureInputAttributeDescription
        };
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

#endif // MODEL_DATA_INCLUDES_H_