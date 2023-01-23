#include "../include/vkInstanceWrapper.h"

vkInstanceWrapper::~vkInstanceWrapper() {
    if (instance != nullptr)
        vkDestroyInstance(*instance, nullptr);
}

void vkInstanceWrapper::listExtensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "available extensions:\n";

    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}

void vkInstanceWrapper::init() {
    VkApplicationInfo appInfo{
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "Momendigm Shift",
        VK_MAKE_VERSION(1, 0, 0),
        "Simz Engine",
        VK_MAKE_VERSION(2, 0, 0),
        VK_API_VERSION_1_0
    };

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo createInfo{
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &appInfo,
        0,
        nullptr,
        glfwExtensionCount,
        glfwExtensions
    };

    instance = new VkInstance();
    if (vkCreateInstance(&createInfo, nullptr, instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}
