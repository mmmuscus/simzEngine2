#ifndef IMGUI_INSTANCE_H_
#define IMGUI_INSTANCE_H_

#include "../../vulkanWrappers/include/generalIncludes.h"
#include "../../vulkanWrappers/include/vulkanSurface.h"

#include <imGui/imgui.h>
#include <imGui/imgui_impl_glfw.h>
#include <imGui/imgui_impl_vulkan.h>

const int IMGUI_MIN_IMAGE_COUNT = 2;

class imGuiInstance {
private:
	// Helper flag
	bool isInstance = false;

	// Vulkan variables
	vk::RenderPass renderPass;
	std::vector<vk::Framebuffer> framebuffers;

	// Not maintained by class
	vk::Device device;

public:
	~imGuiInstance();

	void init(
		GLFWwindow* window,
		vulkanInstance* instance,
		vulkanSurface* surface
	);

	static void checkVkResult(VkResult err);

	void recreateFramebuffers(vulkanSurface* surface);

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

	void destroyFramebuffers();

	void drawGui(vk::CommandBuffer commandBuffer);

};

#endif // IMGUI_INSTANCE_H_
