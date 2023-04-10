#ifndef OBJECT_H_
#define OBJECT_H_

#include "../../vulkanWrappers/include/vulkanObject.h"
#include "../../vulkanWrappers/include/vulkanModelData.h"
#include "../../vulkanWrappers/include/vulkanTextureData.h"

class object {
private:
	// model matrix:
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	// ONLY ROTATES WITH (0, 0, 1) AXIS FOR NOW !!!! 
	// ITS STORED IN THE X AXIS
	// TODO: IMPLEMENT QUATERNIONS
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	// vulkan buffers, pipeline
	vulkanObject* vkObject;
	vulkanModelData* modelData;
	vulkanTextureData* textureData;

public:
	object() : vkObject(nullptr), modelData(nullptr), textureData(nullptr) {}
	object(
		vulkanObject* _vkObject,
		vulkanModelData* _modelData,
		vulkanTextureData* _textureData
	) :
		vkObject(_vkObject),
		modelData(_modelData),
		textureData(_textureData)
	{};

	void setPos(glm::vec3 _pos) { pos = _pos; }
	void setRotationAxis(glm::vec3 _rotation) { rotation = _rotation; }
	void setScale(glm::vec3 _scale) { scale = _scale; }

	void setVulkanObject(vulkanObject* _vkObject) { vkObject = _vkObject; }
	void setModelData(vulkanModelData* _modelData) { modelData = _modelData; }
	void setTextureData(vulkanTextureData* _textureData) { textureData = _textureData; }

	vulkanObject* getVulkanObject() { return vkObject; }
	vulkanModelData* getModelData() { return modelData; }
	vulkanTextureData* getTextureData() { return textureData; }

	void updateTranslationVectors();
	void updateModelTranslation(uint32_t currentFrame);

private:

};

#endif // OBJECT_H_