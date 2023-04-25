#ifndef VULKAN_MODEL_DATA_H_
#define VULKAN_MODEL_DATA_H_

#include "generalIncludes.h"

#include "../../resourceManager/include/meshDataManager.h"
#include "../../resourceManager/include/textureDataManager.h"

#include "vulkanMeshData.h"
#include "vulkanTextureData.h"

class vulkanModelData {
private:
	// Data:
	vulkanMeshData* meshData;
	vulkanTextureData* textureData;
	// Descriptorsets:
	std::vector<vk::DescriptorSet> descriptorSets;

	// Not maintained by class
	vk::Device device;

public:
	void setDevice(vk::Device _device) { device = _device; }
	void setMeshData(vulkanMeshData* _meshData) { meshData = _meshData; }
	void setTextureData(vulkanTextureData* _textureData) { textureData = _textureData; }

	std::vector<vk::DescriptorSet> getDescriptorSets() { return descriptorSets; }
	vulkanMeshData* getMeshData() { return meshData; }
	vulkanTextureData* getTextureData() { return textureData; }

	void init(
		vulkanInstance* instance,
		meshDataManager* meshManager, std::string meshPath, vulkanDynamicUniformBuffer* uniformBuffer,
		textureDataManager* textureManager, std::string texturePath, vulkanTextureSampler* textureSampler,
		vk::DescriptorSetLayout descriptorSetLayout, vk::DescriptorPool descriptorPool
	);
	void init(
		vk::Device _device,
		vulkanMeshData* _meshData,
		vulkanTextureData* _textureData,
		vk::DescriptorSetLayout descriptorSetLayout,
		vk::DescriptorPool descriptorPool
	);
	void initDescriptorSets(
		vk::DescriptorSetLayout descriptorSetLayout,
		vk::DescriptorPool descriptorPool
	);

private:

};

#endif // VULKAN_MODEL_DATA_H_
