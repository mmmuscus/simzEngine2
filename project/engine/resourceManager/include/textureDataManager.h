#ifndef TEXTURE_DATA_MANAGER_H_
#define TEXTURE_DATA_MANAGER_H_

#include "../../vulkanWrappers/include/vulkanTextureData.h"

class textureDataManager {
private:
	std::vector<vulkanTextureData*> textureDatas;

public:
	void destroyList() {
		for (size_t i = 0; i < textureDatas.size(); i++)
			textureDatas[i]->destroy();

		textureDatas.clear();
	}

	std::vector<vulkanTextureData*> getTextureDatas() { return textureDatas; }

	vulkanTextureData* addTextureData(
		std::string texturePath,
		vulkanInstance* instance,
		vulkanTextureSampler* textureSampler
	) {
		for (int i = 0; i < textureDatas.size(); i++)
			if (texturePath == textureDatas[i]->getName())
				return textureDatas[i];

		textureDatas.push_back(new vulkanTextureData());
		textureDatas.back()->init(texturePath, instance, textureSampler);
		return textureDatas.back();
	}

private:

};

#endif // TEXTURE_DATA_MANAGER_H_
