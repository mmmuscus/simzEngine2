#ifndef RENDERER_COMPONENT_H_
#define RENDERER_COMPONENT_H_

#include "../../vulkanWrappers/include/vulkanPipeline.h"
#include "../../vulkanWrappers/include/vulkanMeshData.h"
#include "../../vulkanWrappers/include/vulkanTextureData.h"
#include "../../vulkanWrappers/include/vulkanModelData.h"

#include "../../resourceManager/include/meshDataManager.h"
#include "../../resourceManager/include/textureDataManager.h"

#include "../include/transformComponent.h"

class rendererComponent {
private:
	vulkanPipeline* vkPipeline = nullptr;
	vulkanModelData* modelData = nullptr;

public:
	rendererComponent() : vkPipeline(nullptr), modelData(nullptr) {};
	rendererComponent(vulkanPipeline* _vkPipeline, vulkanModelData* _modelData) :
		vkPipeline(_vkPipeline), modelData(_modelData)
	{};
	rendererComponent(
		vulkanPipeline* _vkPipeline, vk::Device* device,
		meshDataManager* meshManager, size_t meshIndex,
		textureDataManager* textureManager, size_t textureIndex
	) :
		vkPipeline(_vkPipeline), modelData(new vulkanModelData())
	{
		modelData->init(
			device,
			meshManager->getMeshDatas()[meshIndex],
			textureManager->getTextureDatas()[textureIndex],
			vkPipeline->getModelDescriptorSetLayout(), vkPipeline->getModelDescriptorPool()
		);
	}
	rendererComponent(
		vulkanPipeline* _vkPipeline, vulkanInstance* instance,
		meshDataManager* meshManager, std::string meshPath, vulkanDynamicUniformBuffer* uniformBuffer,
		textureDataManager* textureManager, std::string texturePath, vulkanTextureSampler* textureSampler
	) :
		vkPipeline(_vkPipeline), modelData(new vulkanModelData())
	{
		modelData->init(
			instance,
			meshManager, meshPath, uniformBuffer,
			textureManager, texturePath, textureSampler,
			vkPipeline->getModelDescriptorSetLayout(), vkPipeline->getModelDescriptorPool()
		);
	}

	void setVulkanPipeline(vulkanPipeline* _vkPipeline) { vkPipeline = _vkPipeline; }
	void setModelData(vulkanModelData* _modelData) { modelData = _modelData; }

	vulkanPipeline* getVulkanPipeline() { return vkPipeline; }
	vulkanModelData* getModelData() { return modelData; }

	void updateModelTranslation(
		uint32_t currentFrame, transformComponent* transform, uint32_t objectNumber
	);

private:

};

#endif // RENDERER_COMPONENT_H_