#ifndef IMGUI_INSTANCE_H_
#define IMGUI_INSTANCE_H_

#include <filesystem>

#include "imGuiWrappersGeneralIncludes.h"

#include "../../vulkanWrappers/include/vulkanSurface.h"

#include "../../renderLogic/include/scene.h"

#include "../../resourceManager/include/meshDataManager.h"
#include "../../resourceManager/include/textureDataManager.h"

class imGuiInstance {
private:
	// Helper flag
	bool isInstance = false;
	bool isEnabled = false;

	// TODO: move to someplace better
	// Object adding paths
	char meshPath[50] = "models/tank.objj";
	char texturePath[50] = "textures/damn_texture.jpeg";
	// Manager selectors
	int currentMeshItem = 0;
	int currentTextureItem = 0;

	// Vulkan variables
	vk::RenderPass renderPass;
	std::vector<vk::Framebuffer> framebuffers;

	// Not maintained by class
	vk::Device device;

public:
	~imGuiInstance();

	bool getIsEnabled() { return isEnabled; }

	void setIsEnabled(bool _isEnabled) { isEnabled = _isEnabled; }

	void init(
		GLFWwindow* window,
		vulkanInstance* instance,
		vulkanSurface* surface
	);

	void recreateFramebuffers(vulkanSurface* surface);

	void presentGui(
		bool shouldRecreateSwapChain, scene* currScene,
		vulkanInstance* instance, vulkanObject* obj,
		meshDataManager* meshManager, textureDataManager* textureManager, 
		vulkanDynamicUniformBuffer* buffer, vulkanTextureSampler* sampler
	);

	void drawFrame(
		vulkanSurface* surface,
		vulkanInstance* instance,
		uint32_t imageIndex
	);

private:
	vk::DescriptorPool initDescriptorPool();
	void initRenderPass(vk::Format format);
	void initFramebuffers(vulkanSurface* surface);
	void initImGui(GLFWwindow* window, vulkanInstance* instance);
	static void checkVkResult(VkResult err);

	void destroyFramebuffers();

	void showGui(
		scene* currScene,
		vulkanInstance* instance, vulkanObject* obj,
		meshDataManager* meshManager, textureDataManager* textureManager,
		vulkanDynamicUniformBuffer* buffer, vulkanTextureSampler* sampler
	);
	void showObjectGui(object* obj);

};

#endif // IMGUI_INSTANCE_H_
