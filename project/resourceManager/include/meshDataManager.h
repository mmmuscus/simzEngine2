#ifndef MESH_DATA_MANAGER_H_
#define MESH_DATA_MANAGER_H_

#include "../../vulkanWrappers/include/vulkanMeshData.h"

class meshDataManager {
private:
	std::vector<vulkanMeshData*> meshDatas;

public:
	std::vector<vulkanMeshData*> getMeshDatas() { return meshDatas; }

	vulkanMeshData* addMeshData(
		std::string meshPath,
		vulkanInstance* instance,
		vulkanDynamicUniformBuffer* uniformBuffer
	) {
		meshDatas.push_back(new vulkanMeshData());
		meshDatas.back()->init(meshPath, instance, uniformBuffer);
		return meshDatas.back();
	}

private:


};

#endif // MESH_DATA_MANAGER_H_
