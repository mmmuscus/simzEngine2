#ifndef VK_INSTANCE_WRAPPER_H_
#define VK_INSTANCE_WRAPPER_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>

class vkInstanceWrapper {
private:
	VkInstance* instance;

public:
	vkInstanceWrapper() : instance(nullptr) {}
	~vkInstanceWrapper();

	VkInstance* get() { return instance; }

	void listExtensions();
	void init();
};

#endif /* VK_INSTANCE_WRAPPER_H_ */

