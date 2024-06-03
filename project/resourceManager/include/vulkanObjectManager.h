#ifndef VULKAN_OBJECT_MANAGER
#define VULKAN_OBJECT_MANAGER

#include "../../vulkanWrappers/include/vulkanObject.h"

class vulkanObjectManager {
private:
	std::vector<vulkanObject*> vulkanObjects;

public:
	void destroyList() {
		for (size_t i = 0; i < vulkanObjects.size(); i++)
			vulkanObjects[i]->destroy();

		vulkanObjects.clear();
	}

	std::vector<vulkanObject*> getVulkanObjects() { return vulkanObjects; }

	vulkanObject* addVulkanObject(
		std::string name, vk::Device* device,
		vk::Extent2D extent, vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSamples,
		std::string vertexShaderPath, std::string fragmentShaderPath
	) {
		// TODO:
		// What if már benne van ez a vkObject?

		vulkanObjects.push_back(new vulkanObject());
		vulkanObjects.back()->init(
			name, device,
			extent, renderPass, msaaSamples,
			vertexShaderPath, fragmentShaderPath
		);
		return vulkanObjects.back();
	}

private:

};

#endif // VULKAN_OBJECT_MANAGER
