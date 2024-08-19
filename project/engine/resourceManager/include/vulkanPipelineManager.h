#ifndef VULKAN_PIPELINE_MANAGER
#define VULKAN_PIPELINE_MANAGER

#include "../../vulkanWrappers/include/vulkanPipeline.h"

class vulkanPipelineManager {
private:
	std::vector<vulkanPipeline*> vulkanPipelines;

public:
	void destroyList() {
		for (size_t i = 0; i < vulkanPipelines.size(); i++)
			vulkanPipelines[i]->destroy();

		vulkanPipelines.clear();
	}

	std::vector<vulkanPipeline*> getVulkanPipelines() { return vulkanPipelines; }

	vulkanPipeline* addVulkanPipeline(
		std::string name, vk::Device* device,
		vk::Extent2D extent, vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSamples,
		std::string vertexShaderPath, std::string fragmentShaderPath
	) {
		for (int i = 0; i < vulkanPipelines.size(); i++)
			if (name == vulkanPipelines[i]->getName())
				return vulkanPipelines[i];

		vulkanPipelines.push_back(new vulkanPipeline());
		vulkanPipelines.back()->init(
			name, device,
			extent, renderPass, msaaSamples,
			vertexShaderPath, fragmentShaderPath
		);
		return vulkanPipelines.back();
	}

private:

};

#endif // VULKAN_PIPELINE_MANAGER
