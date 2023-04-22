#ifndef IMGUI_INSTANCE_H_
#define IMGUI_INSTANCE_H_

#include "imGuiWrappersGeneralIncludes.h"

#include "../../vulkanWrappers/include/vulkanSurface.h"

class imGuiInstance {
private:
	// Helper flag
	bool isInstance = false;
	bool isEnabled = false;

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

	void drawGui(vk::CommandBuffer commandBuffer);

};

#endif // IMGUI_INSTANCE_H_
