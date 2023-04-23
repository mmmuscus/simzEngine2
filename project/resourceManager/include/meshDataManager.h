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
		// TODO: Fix
		/*std::string dummyName = meshPath.substr(meshPath.find_last_of("/\\") + 1);
		std::string::size_type lastDot(dummyName.find_last_of('.'));
		dummyName = dummyName.substr(0, lastDot);

		for (size_t i = 0; i < meshDatas.size(); i++) 
			if (dummyName.compare(meshDatas[i]->getName()))
				return meshDatas[i];*/

		meshDatas.push_back(new vulkanMeshData());
		meshDatas.back()->init(meshPath, instance, uniformBuffer);
		return meshDatas.back();
	}

private:

};

#endif // MESH_DATA_MANAGER_H_
