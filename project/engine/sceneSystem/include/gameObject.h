#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#define _USE_MATH_DEFINES

#include "../../general/include/timer.h"
#include "../../general/include/Quat.h"

#include "../../vulkanWrappers/include/vulkanPipeline.h"
#include "../../vulkanWrappers/include/vulkanMeshData.h"
#include "../../vulkanWrappers/include/vulkanTextureData.h"
#include "../../vulkanWrappers/include/vulkanModelData.h"

#include "../../resourceManager/include/meshDataManager.h"
#include "../../resourceManager/include/textureDataManager.h"

class gameObject {
private:
	// Transform vectors:
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	// TODO: IMPLEMENT QUATERNIONS
	Quat quaternion = Quat(0.0f, 0.0f, 0.0f, 1.0f);
	// Needed for converting Quaternion to Euler
	bool outsideRange = false;
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	// Model matrix:
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	// Helper for uploading the dynamic uniformBuffer
	uint32_t objectNumber;

	// timer
	timer* sceneTimer;

	// vulkan buffers, pipeline
	vulkanPipeline* vkPipeline;
	vulkanModelData* modelData;

public:
	gameObject() : sceneTimer(nullptr), vkPipeline(nullptr), modelData(nullptr) {}
	gameObject(
		vulkanPipeline* _vkPipeline,
		vulkanModelData* _modelData,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		sceneTimer(nullptr),
		vkPipeline(_vkPipeline), modelData(_modelData),
		pos(_pos), scale(_scale)
	{
		setOutsideRange(_rotation.y);
		quaternion = Quat::fromEuler(_rotation);
	};

	gameObject(
		vk::Device* device, vulkanPipeline* _vkPipeline,
		meshDataManager* meshManager, size_t meshIndex,
		textureDataManager* textureManager, size_t textureIndex,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		sceneTimer(nullptr),
		vkPipeline(_vkPipeline), modelData(new vulkanModelData()),
		pos(_pos), scale(_scale)
	{
		setOutsideRange(_rotation.y);
		quaternion = Quat::fromEuler(_rotation);

		modelData->init(
			device,
			meshManager->getMeshDatas()[meshIndex],
			textureManager->getTextureDatas()[textureIndex],
			vkPipeline->getModelDescriptorSetLayout(), vkPipeline->getModelDescriptorPool()
		);
		calculateModelMatrix();
	}
	gameObject(
		vulkanInstance* instance, vulkanPipeline* _vkPipeline,
		meshDataManager* meshManager, std::string meshPath, vulkanDynamicUniformBuffer* uniformBuffer,
		textureDataManager* textureManager, std::string texturePath, vulkanTextureSampler* textureSampler,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		sceneTimer(nullptr),
		vkPipeline(_vkPipeline), modelData(new vulkanModelData()),
		pos(_pos), scale(_scale)
	{
		setOutsideRange(_rotation.y);
		quaternion = Quat::fromEuler(_rotation);

		modelData->init(
			instance,
			meshManager, meshPath, uniformBuffer,
			textureManager, texturePath, textureSampler,
			vkPipeline->getModelDescriptorSetLayout(), vkPipeline->getModelDescriptorPool()
		);
		calculateModelMatrix();
	}

	void setPos(glm::vec3 _pos) { pos = _pos; }
	void setQuaternion(Quat _quaternion) { quaternion = _quaternion.normalized(); }
	void setScale(glm::vec3 _scale) { scale = _scale; }
	void setObjectNumber(uint32_t _objectNumber) { objectNumber = _objectNumber; }
	void setSceneTimer(timer* _sceneTimer) { sceneTimer = _sceneTimer; }
	void setVulkanPipeline(vulkanPipeline* _vkPipeline) { vkPipeline = _vkPipeline; }
	void setModelData(vulkanModelData* _modelData) { modelData = _modelData; }

	glm::vec3 getPos() { return pos; }
	Quat getQuaternion() { return quaternion; }
	bool getOutsideRange() { return outsideRange; }
	glm::vec3 getScale() { return scale; }
	uint32_t getObjectNumber() { return objectNumber; }
	vulkanPipeline* getVulkanPipeline() { return vkPipeline; }
	vulkanModelData* getModelData() { return modelData; }

	void setOutsideRange(float y);
	glm::vec3 getEuler();
	glm::vec3 getEulerWithFlag();
	void calculateModelMatrix();
	void updateModelTranslation(uint32_t currentFrame);

private:

};

#endif // GAME_OBJECT_H_
