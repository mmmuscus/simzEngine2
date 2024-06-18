#ifndef MESH_DATA_MANAGER_H_
#define MESH_DATA_MANAGER_H_

#include "../../vulkanWrappers/include/vulkanMeshData.h"

class meshDataManager {
private:
	std::vector<vulkanMeshData*> meshDatas;

public:
	void destroyList() {
		for (size_t i = 0; i < meshDatas.size(); i++)
			meshDatas[i]->destroy();

		meshDatas.clear();
	}

	std::vector<vulkanMeshData*> getMeshDatas() { return meshDatas; }

	vulkanMeshData* addMeshData(
		std::string meshPath,
		vulkanInstance* instance,
		vulkanDynamicUniformBuffer* uniformBuffer
	) {
		for (int i = 0; i < meshDatas.size(); i++) 
			if (meshPath == meshDatas[i]->getName())
				return meshDatas[i];

		meshDatas.push_back(new vulkanMeshData());
		meshDatas.back()->init(meshPath, instance, uniformBuffer);
		return meshDatas.back();
	}

private:

};

#endif // MESH_DATA_MANAGER_H_
