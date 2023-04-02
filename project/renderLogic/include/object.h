#ifndef OBJECT_H_
#define OBJECT_H_

#include "../../vulkanWrappers/include/vulkanObject.h"
#include "../../vulkanWrappers/include/vulkanModelData.h"
#include "../../vulkanWrappers/include/vulkanTextureData.h"

class object {
private:
	// model matrix:
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	// vulkan buffers, pipeline
	vulkanObject* vkObject;
	vulkanModelData* modelData;
	vulkanTextureData* textureData;

public:
	void setPos(glm::vec3 _pos) { pos = _pos; }
	void setRotation(glm::vec3 _rotation) { rotation = _rotation; }
	void setScale(glm::vec3 _scale) { scale = _scale; }

	void setVulkanObject(vulkanObject* _vkObject) { vkObject = _vkObject; }
	void setModelData(vulkanModelData* _modelData) { modelData = _modelData; }
	void setTextureData(vulkanTextureData* _textureData) { textureData = _textureData; }

	vulkanObject* getVulkanObject() { return vkObject; }
	vulkanModelData* getModelData() { return modelData; }
	vulkanTextureData* getTextureData() { return textureData; }

	void updateModelTranslation();

private:

};

#endif // OBJECT_H_
