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
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkDevice* device;

public:
	vkSwapChainWrapper() : swapChain(nullptr), device(nullptr) {}
	~vkSwapChainWrapper();

	void init(vkDeviceWrapper* deviceWrapper,  GLFWwindow* window, VkSurfaceKHR* surface);

private:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& abailablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
};

#endif // VK_SWAP_CHAIN_WRAPPER_H_