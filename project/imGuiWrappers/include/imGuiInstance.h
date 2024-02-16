#ifndef IMGUI_INSTANCE_H_
#define IMGUI_INSTANCE_H_

#include <filesystem>

#include "imGuiWrappersGeneralIncludes.h"

#include "../../vulkanWrappers/include/vulkanSurface.h"

#include "../../renderLogic/include/scene.h"

#include "../../resourceManager/include/meshDataManager.h"
#include "../../resourceManager/include/textureDataManager.h"
#include "../../resourceManager/include/vulkanObjectManager.h"

// The base implementation of the class havily relies on this tutorial:
// https://frguthmann.github.io/posts/vulkan_imgui/#IMGUI_RENDER_PASS_INITIAL_LAYOUT
// Reference of ImGui integration with Vulkan and glfw:
// https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
class imGuiInstance {
private:
	bool isCreated = false;

	// Vulkan components:
	vk::DescriptorPool descriptorPool = VK_NULL_HANDLE;
	vk::RenderPass renderPass = VK_NULL_HANDLE;
	std::vector<vk::Framebuffer> framebuffers;

	// Not maintained by the class:
	vk::Device device;

public:
	~imGuiInstance();
	void destroy();
	void destroyFramebuffers();

	void init(GLFWwindow* _window, vulkanInstance* _instance, vulkanSurface* _surface);
	
	void recreateFramebuffers(vulkanSurface* _surface);

	void drawGui();
	void drawFrame(vk::CommandBuffer _commandBuffer, vulkanSurface* _surface, uint32_t imageIndex);

private:
	void initDescriptorPool();
	void initRenderPass(vk::Format _format);
	void initFramebuffers(vulkanSurface* _surface);
};

#endif // IMGUI_INSTANCE_H_
