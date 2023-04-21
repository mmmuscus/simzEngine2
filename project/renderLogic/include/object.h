#ifndef OBJECT_H_
#define OBJECT_H_

#include "../../general/include/timer.h"

#include "../../vulkanWrappers/include/vulkanObject.h"
#include "../../vulkanWrappers/include/vulkanModelData.h"
#include "../../vulkanWrappers/include/vulkanTextureData.h"

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

	// Descriptor set:
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;
	// Helper for descturctors
	vk::Device device;

	// vulkan buffers, pipeline
	vulkanObject* vkObject;
	vulkanModelData* modelData;
	vulkanTextureData* textureData;

public:
	~object();

	object() : sceneTimer(nullptr), vkObject(nullptr), modelData(nullptr), textureData(nullptr) {}
	object(
		vulkanObject* _vkObject,
		vulkanModelData* _modelData,
		vulkanTextureData* _textureData
	) :
		sceneTimer(nullptr),
		vkObject(_vkObject),
		modelData(_modelData),
		textureData(_textureData)
	{ initDescriptors(); };

	void setPos(glm::vec3 _pos) { pos = _pos; }
	void setRotation(glm::vec3 _rotation) { rotation = _rotation; }
	void setScale(glm::vec3 _scale) { scale = _scale; }
	void setObjectNumber(uint32_t _objectNumber) { objectNumber = _objectNumber; }
	void setSceneTimer(timer* _sceneTimer) { sceneTimer = _sceneTimer; }
	void setVulkanObject(vulkanObject* _vkObject) { vkObject = _vkObject; }
	void setModelData(vulkanModelData* _modelData) { modelData = _modelData; }
	void setTextureData(vulkanTextureData* _textureData) { textureData = _textureData; }

	uint32_t getObjectNumber() { return objectNumber; }
	std::vector<vk::DescriptorSet> getDescriptorSets() { return descriptorSets; }
	vulkanObject* getVulkanObject() { return vkObject; }
	vulkanModelData* getModelData() { return modelData; }
	vulkanTextureData* getTextureData() { return textureData; }

	void initDescriptors();

	void updateTranslationVectors();
	void updateModelTranslation(uint32_t currentFrame);

private:

	void initDescriptorPool();
	void initDescriptorSets();

};

#endif // OBJECT_H_
