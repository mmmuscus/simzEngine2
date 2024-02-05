#ifndef IMGUI_INSTANCE_H_
#define IMGUI_INSTANCE_H_

#include <filesystem>

#include "imGuiWrappersGeneralIncludes.h"

#include "../../vulkanWrappers/include/vulkanSurface.h"

#include "../../renderLogic/include/scene.h"

#include "../../resourceManager/include/meshDataManager.h"
#include "../../resourceManager/include/textureDataManager.h"
#include "../../resourceManager/include/vulkanObjectManager.h"

// Reference of ImGui integration with Vulkan and glfw:
// https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
class imGuiInstance {
private:
	bool isCreated = false;

	// Vulkan components:
	vk::DescriptorPool descriptorPool = VK_NULL_HANDLE;
	vk::RenderPass renderPass = VK_NULL_HANDLE;

	// Not maintained by the class:
	vk::Device device;

public:
	~imGuiInstance();
	void destroy();

	void init(GLFWwindow* _window, vulkanInstance* _instance, vulkanSurface* _surface);

private:
	void initDescriptorPool();
	void initRenderPass(vk::Format _format);
};

#endif // IMGUI_INSTANCE_H_
