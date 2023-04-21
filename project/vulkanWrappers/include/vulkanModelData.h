#ifndef VULKAN_MODEL_DATA_H_
#define VULKAN_MODEL_DATA_H_

#include "generalIncludes.h"

#include "vulkanMeshData.h"
#include "vulkanTextureData.h"

class vulkanModelData {
private:
	// Data:
	vulkanMeshData* meshData;
	vulkanTextureData* textureData;
	// Descriptorsets:
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;

	// Not maintained by class
	vk::Device device;

public:
	~vulkanModelData();

	void setDevice(vk::Device _device) { device = _device; }
	void setMeshData(vulkanMeshData* _meshData) { meshData = _meshData; }
	void setTextureData(vulkanTextureData* _textureData) { textureData = _textureData; }

	std::vector<vk::DescriptorSet> getDescriptorSets() { return descriptorSets; }
	vulkanMeshData* getMeshData() { return meshData; }
	vulkanTextureData* getTextureData() { return textureData; }

	void initDescriptorPool();
	void initDescriptorSets(vk::DescriptorSetLayout descriptorSetLayout);

private:

};

#endif // VULKAN_MODEL_DATA_H_
