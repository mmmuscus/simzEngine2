#ifndef VULKAN_SURFACE_H_
#define VULKAN_SURFACE_H_

#include "vulkanIncludes.h"

#include "vulkanInstance.h"

class vulkanSurface {
private:
	// Surface:
	vk::SurfaceKHR surface;
	// SwapChain:
	vk::SwapchainKHR swapChain;
	// Images:
	std::vector<vk::Image> images;
	vk::Format imageFormat;
	vk::Extent2D extent;
	// An ImageView is a View to an image (like databases)
	std::vector<vk::ImageView> imageViews;

	// Not maintained by the class:
	vk::Instance instance;
	vk::Device device;

public:
	~vulkanSurface();

	void setInstance(vk::Instance _instance) { instance = _instance; }
	void setDevice(vk::Device _device) { device = _device; }

	vk::SurfaceKHR getSurface() { return surface; }
	vk::SwapchainKHR getSwapChain() { return swapChain; }
	vk::Format getFormat() { return imageFormat; }
	vk::Extent2D getExtent() { return extent; }
	std::vector<vk::ImageView> getImageViews() { return imageViews; }

	void initSurface(GLFWwindow* window);
	void initSwapChain(vulkanInstance* inst, GLFWwindow* window);
	void initImageViews();

private:

	// The format specifies the color channels and types
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	// The presentation mode specifies, how the rendered images are being presented
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
	// The extent is the resolution of the swapchain images
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
};

#endif // VULKAN_SURFACE_H_