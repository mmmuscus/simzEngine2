#ifndef OBJECT_H_
#define OBJECT_H_

#include "../../vulkanWrappers/include/vulkanObject.h"
#include "../../vulkanWrappers/include/vulkanModelData.h"
#include "../../vulkanWrappers/include/vulkanTextureData.h"

class object {
private:
	vulkanObject* vkObject;
	vulkanModelData* modelData;
	vulkanTextureData* textureData;

public:
	void setVulkanObject(vulkanObject* _vkObject) { vkObject = _vkObject; }
	void setModelData(vulkanModelData* _modelData) { modelData = _modelData; }
	void setTextureData(vulkanTextureData* _textureData) { textureData = _textureData; }

	vulkanObject* getVulkanObject() { return vkObject; }
	vulkanModelData* getModelData() { return modelData; }
	vulkanTextureData* getTextureData() { return textureData; }

private:

};

#endif // OBJECT_H_
