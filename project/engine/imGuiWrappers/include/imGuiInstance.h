#ifndef IMGUI_INSTANCE_H_
#define IMGUI_INSTANCE_H_

#include <filesystem>

#include "imGuiWrappersGeneralIncludes.h"

#include "../../general/include/Quat.h"

#include "../../vulkanWrappers/include/vulkanSurface.h"

#include "../../sceneSystem/include/scene.h"

#include "../../resourceManager/include/meshDataManager.h"
#include "../../resourceManager/include/textureDataManager.h"
#include "../../resourceManager/include/vulkanPipelineManager.h"

// The base implementation of the class havily relies on this tutorial:
// https://frguthmann.github.io/posts/vulkan_imgui
// Reference of ImGui integration with Vulkan and glfw:
// https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
class imGuiInstance {
private:
	bool isCreated = false;

	// Vulkan components:
	vk::DescriptorPool descriptorPool = VK_NULL_HANDLE;
	vk::RenderPass renderPass = VK_NULL_HANDLE;
	std::vector<vk::Framebuffer> framebuffers;
	vk::CommandPool commandPool = VK_NULL_HANDLE;
	std::vector<vk::CommandBuffer> commandBuffers;

	// Not maintained by the class:
	vk::Device* device;

	// Variables for editor:
	// Paths:
	char meshPath[50] = "assets/models/tank.objj";
	char texturePath[50] = "assets/textures/damn_texture.jpeg";
	// Manager selectors:
	int currentMeshItem = 0;
	int currentTextureItem = 0;
	int currentVulkanPipelineItem = 0;

	// Managers:
	vulkanPipelineManager* pipelineManager;
	meshDataManager* meshManager;
	textureDataManager* textureManager;

public:
	~imGuiInstance();
	void destroy();
	void destroyFramebuffers();

	void init(GLFWwindow* _window, vulkanInstance* _instance, vulkanSurface* _surface,
		vulkanPipelineManager* _pipelineManager, meshDataManager* _meshManager, textureDataManager* _textureManager
	);
	
	void recreateFramebuffers(vulkanSurface* _surface);

	void drawGui(
		scene* currScene, vulkanInstance* instance, vulkanPipeline* pipeline,
		vulkanDynamicUniformBuffer* buffer, vulkanTextureSampler* sampler
	);
	void drawFrame(vulkanSurface* _surface, uint32_t imageIndex, size_t currentFrame);

private:
	void initDescriptorPool();
	void initRenderPass(vk::Format _format);
	void initFramebuffers(vulkanSurface* _surface);
	void initCommandPool(QueueFamilyIndices queueFamilyIndices);
	void initCommandBuffers();

	void initEditor(vulkanPipelineManager* _pipelineManager,
		meshDataManager* _meshManager, textureDataManager* _textureManager
	);

	void showGui(
		scene* currScene,
		vulkanInstance* instance, vulkanPipeline* pipeline,
		vulkanDynamicUniformBuffer* buffer, vulkanTextureSampler* sampler
	);
	void showObjectEditGui(gameObject* obj);
};

#endif // IMGUI_INSTANCE_H_
