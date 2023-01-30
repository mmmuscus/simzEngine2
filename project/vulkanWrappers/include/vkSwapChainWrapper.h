#ifndef VK_SWAP_CHAIN_WRAPPER_H_
#define VK_SWAP_CHAIN_WRAPPER_H_

#include "generalIncludes.h"

#include <limits>
#include <algorithm>

#include "vkDeviceWrapper.h"

class vkSwapChainWrapper {
private:
	VkSwapchainKHR* swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat* swapChainImageFormat;
	VkExtent2D* swapChainExtent;
	// An ImageView is a View to an image (think databases)
	std::vector<VkImageView> swapChainImageViews;

	// The following resources are not being maintained by the class
	VkDevice* device;

public:
	vkSwapChainWrapper() : swapChain(nullptr), device(nullptr) {}
	~vkSwapChainWrapper();

	VkFormat* getImageFormat() { return swapChainImageFormat; }

	void init(vkDeviceWrapper* deviceWrapper,  GLFWwindow* window, VkSurfaceKHR* surface);

private:
	void initSwapChain(vkDeviceWrapper* deviceWrapper, GLFWwindow* window, VkSurfaceKHR* surface);
	void initImageViews();

	// The format specifies the color channels and types
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	// The presentation mode specifies, how the rendered images are being presented
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& abailablePresentModes);
	// The extent is the resolution of the swapchain images
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
};

#endif // VK_SWAP_CHAIN_WRAPPER_H_