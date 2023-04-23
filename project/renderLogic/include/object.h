#ifndef OBJECT_H_
#define OBJECT_H_

#include "../../general/include/timer.h"

#include "../../vulkanWrappers/include/vulkanObject.h"
#include "../../vulkanWrappers/include/vulkanMeshData.h"
#include "../../vulkanWrappers/include/vulkanTextureData.h"
#include "../../vulkanWrappers/include/vulkanModelData.h"

#include "../../resourceManager/include/meshDataManager.h"
#include "../../resourceManager/include/textureDataManager.h"

class object {
private:
	// model matrix:
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	// TODO: IMPLEMENT QUATERNIONS
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	// Helper for uploading the dynamic uniformBuffer
	uint32_t objectNumber;

	// timer
	timer* sceneTimer;

	// vulkan buffers, pipeline
	vulkanObject* vkObject;
	vulkanModelData* modelData;

public:
	object() : sceneTimer(nullptr), vkObject(nullptr), modelData(nullptr) {}
	object(
		vulkanObject* _vkObject,
		vulkanModelData* _modelData,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		sceneTimer(nullptr),
		vkObject(_vkObject), modelData(_modelData),
		pos(_pos), rotation(_rotation), scale(_scale)
	{};
	object(
		vk::Device device, vulkanObject* _vkObject,
		meshDataManager* meshManager, size_t meshIndex,
		textureDataManager* textureManager, size_t textureIndex,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		sceneTimer(nullptr),
		vkObject(_vkObject), modelData(new vulkanModelData()),
		pos(_pos), rotation(_rotation), scale(_scale)
	{
		modelData->init(
			device,
			meshManager->getMeshDatas()[meshIndex],
			textureManager->getTextureDatas()[textureIndex],
			vkObject->getModelDescriptorSetLayout()
		);
	}
	object(
		vulkanInstance* instance, vulkanObject* _vkObject,
		meshDataManager* meshManager, std::string meshPath, vulkanDynamicUniformBuffer* uniformBuffer,
		textureDataManager* textureManager, std::string texturePath, vulkanTextureSampler* textureSampler,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		sceneTimer(nullptr),
		vkObject(_vkObject), modelData(new vulkanModelData()),
		pos(_pos), rotation(_rotation), scale(_scale)
	{
		modelData->init(
			instance,
			meshManager, meshPath, uniformBuffer,
			textureManager, texturePath, textureSampler,
			vkObject->getModelDescriptorSetLayout()
		);
	}

	void setPos(glm::vec3 _pos) { pos = _pos; }
	void setRotation(glm::vec3 _rotation) { rotation = _rotation; }
	void setScale(glm::vec3 _scale) { scale = _scale; }
	void setObjectNumber(uint32_t _objectNumber) { objectNumber = _objectNumber; }
	void setSceneTimer(timer* _sceneTimer) { sceneTimer = _sceneTimer; }
	void setVulkanObject(vulkanObject* _vkObject) { vkObject = _vkObject; }
	void setModelData(vulkanModelData* _modelData) { modelData = _modelData; }

	glm::vec3 getPos() { return pos; }
	glm::vec3 getRotation() { return rotation; }
	glm::vec3 getScale() { return scale; }
	uint32_t getObjectNumber() { return objectNumber; }
	vulkanObject* getVulkanObject() { return vkObject; }
	vulkanModelData* getModelData() { return modelData; }

	void updateTranslationVectors();
	void updateModelTranslation(uint32_t currentFrame);

private:

};

#endif // OBJECT_H_
