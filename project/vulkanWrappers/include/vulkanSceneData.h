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
	std::vector<vk::Buffer> sceneBuffers;
	std::vector<vk::DeviceMemory> sceneBuffersMemory;
	std::vector<void*> sceneBuffersMapped;

	// Not maintained by the class:
	vk::Device device;

public:
	~vulkanSceneData();

	void setDevice(vk::Device _device) { device = _device; }

	void initSceneBuffers(vulkanInstance* instance);

	void updateSceneBuffer(uint32_t currentImage, vk::Extent2D extent, glm::mat4 viewMat);

private:

};

#endif // VULKAN_SCENE_DATA_H_
