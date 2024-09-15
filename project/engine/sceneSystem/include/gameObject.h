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

#include "../include/transformComponent.h"
#include "../include/rendererComponent.h"

class gameObject {
private:
	// Helper for uploading the dynamic uniformBuffer
	uint32_t objectNumber = 0;

	// timer
	timer* sceneTimer = nullptr;

	// Transform component:
	transformComponent* transform = nullptr;
	// Renderer component:
	rendererComponent* renderer = nullptr;

public:
	gameObject() : 
		objectNumber(0), 
		sceneTimer(nullptr), 
		transform(nullptr), 
		renderer(nullptr) 
	{}
	gameObject(
		vulkanPipeline* _vkPipeline,
		vulkanModelData* _modelData,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		objectNumber(0), sceneTimer(nullptr), 
		transform(new transformComponent(_pos, _rotation, _scale)),
		renderer(new rendererComponent(_vkPipeline, _modelData))
	{
		transform->calculateModelMatrix();
	};

	gameObject(
		vk::Device* device, vulkanPipeline* _vkPipeline,
		meshDataManager* meshManager, size_t meshIndex,
		textureDataManager* textureManager, size_t textureIndex,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		objectNumber(0), sceneTimer(nullptr), 
		transform(new transformComponent(_pos, _rotation, _scale)),
		renderer(new rendererComponent(
			_vkPipeline, device,  meshManager, meshIndex, textureManager, textureIndex
		))
	{}
	gameObject(
		vulkanInstance* instance, vulkanPipeline* _vkPipeline,
		meshDataManager* meshManager, std::string meshPath, vulkanDynamicUniformBuffer* uniformBuffer,
		textureDataManager* textureManager, std::string texturePath, vulkanTextureSampler* textureSampler,
		glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) :
		objectNumber(0), sceneTimer(nullptr), 
		transform(new transformComponent(_pos, _rotation, _scale)),
		renderer(new rendererComponent(
			_vkPipeline, instance,
			meshManager, meshPath, uniformBuffer,
			textureManager, texturePath, textureSampler

		))
	{}

	void setObjectNumber(uint32_t _objectNumber) { objectNumber = _objectNumber; }
	void setSceneTimer(timer* _sceneTimer) { sceneTimer = _sceneTimer; }

	uint32_t getObjectNumber() { return objectNumber; }
	transformComponent* getTransform() { return transform; }
	rendererComponent* getRenderer() { return renderer; }

private:

};

#endif // GAME_OBJECT_H_
