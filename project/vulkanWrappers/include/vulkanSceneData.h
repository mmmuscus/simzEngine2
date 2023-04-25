#ifndef VULKAN_SCENE_DATA_H_
#define VULKAN_SCENE_DATA_H_

#include <chrono>

#include "generalIncludes.h"

#include "vulkanInstance.h"

struct sceneUniformBufferObject {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class vulkanSceneData {
private:
	std::vector<vk::Buffer> uniformBuffers;
	std::vector<vk::DeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	// Descriptor Sets
	std::vector<vk::DescriptorSet> descriptorSets;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanSceneData();

	void setDevice(vk::Device _device) { device = _device; }

	std::vector<vk::DescriptorSet> getDescriptorSets() { return descriptorSets; }

	std::vector<vk::Buffer> getUniformBuffers() { return uniformBuffers; }

	void init(
		vulkanInstance* instance,
		vk::DescriptorSetLayout descriptorSetLayout,
		vk::DescriptorPool descriptorPool
	);
	void initUniformBuffers(vulkanInstance* instance);
	void initDescriptorSets(
		vk::DescriptorSetLayout descriptorSetLayout,
		vk::DescriptorPool commandPool
	);

	void updateSceneUniformBuffer(uint32_t currentImage, vk::Extent2D extent, glm::mat4 viewMat);

private:

};

#endif // VULKAN_SCENE_DATA_H_
