#ifndef VULKAN_SURFACE_H_
#define VULKAN_SURFACE_H_

#include "generalIncludes.h"

#include "vulkanInstance.h"
#include "vulkanRenderer.h"

class vulkanSurface {
private:
	// Surface:
	vk::SurfaceKHR surface = VK_NULL_HANDLE;
	// SwapChain:
	vk::SwapchainKHR swapChain = VK_NULL_HANDLE;
	// Images:
	std::vector<vk::Image> images;
	vk::Format imageFormat;
	vk::Extent2D extent;
	// An ImageView is a View to an image (like databases)
	std::vector<vk::ImageView> imageViews;

	// flags:
	bool shouldRecreateSwapChain = false;

	// Not maintained by the class:
	GLFWwindow* window;
	vk::Instance instance;
	vk::Device device;

public:
	~vulkanSurface();
	void destroyImageViews();
	void destroySwapChain();
	void destroySurface();

	bool getShouldRecreateSwapChain() { return shouldRecreateSwapChain; }

	void setShouldRecreateSwapChain(bool _shouldRecreateSwapChain) { shouldRecreateSwapChain = _shouldRecreateSwapChain; }
	void setWindow(GLFWwindow* _window) { window = _window; }
	void setInstance(vk::Instance _instance) { instance = _instance; }
	void setDevice(vk::Device _device) { device = _device; }

	vk::SurfaceKHR getSurface() { return surface; }
	vk::SwapchainKHR getSwapChain() { return swapChain; }
	vk::Format getFormat() { return imageFormat; }
	vk::Extent2D getExtent() { return extent; }
	std::vector<vk::ImageView> getImageViews() { return imageViews; }

	void initSurface();
	void initSwapChain(vulkanInstance* inst);
	void initImageViews(vulkanInstance* instance);

	void cleanupSwapChain(vulkanRenderer* renderer);
	void recreateSwapChain(vulkanRenderer* renderer, vulkanInstance* inst);

private:

	// The format specifies the color channels and types
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	// The presentation mode specifies, how the rendered images are being presented
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
	// The extent is the resolution of the swapchain images
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
};

#endif // VULKAN_SURFACE_H_