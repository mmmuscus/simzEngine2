#version 450

#extension GL_KHR_vulkan_glsl : enable

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 baseColor = texture(texSampler, fragTexCoord);
    outColor = vec4(1.0f - baseColor.x, 1.0f - baseColor.y, 1.0f - baseColor.z, baseColor.w);
}