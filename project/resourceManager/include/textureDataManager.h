#ifndef TEXTURE_DATA_MANAGER_H_
#define TEXTURE_DATA_MANAGER_H_

#include "../../vulkanWrappers/include/vulkanTextureData.h"

class textureDataManager {
private:
	std::vector<vulkanTextureData*> textureDatas;

public:
	std::vector<vulkanTextureData*> getTextureDatas() { return textureDatas; }

	vulkanTextureData* addTextureData(
		std::string texturePath,
		vulkanInstance* instance,
		vulkanTextureSampler* textureSampler
	) {
		// TODO: Fix
		/*std::string dummyName = texturePath.substr(texturePath.find_last_of("/\\") + 1);
		std::string::size_type lastDot(dummyName.find_last_of('.'));
		dummyName = dummyName.substr(0, lastDot);

		for (size_t i = 0; i < textureDatas.size(); i++)
			if (dummyName.compare(textureDatas[i]->getName()))
				return textureDatas[i];*/

		textureDatas.push_back(new vulkanTextureData());
		textureDatas.back()->init(texturePath, instance, textureSampler);
		return textureDatas.back();
	}

private:

};

#endif // TEXTURE_DATA_MANAGER_H_
