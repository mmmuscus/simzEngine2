#ifndef GENERAL_INCLUDES_H_
#define GENERAL_INCLUDES_H_

#include <stdexcept>
#include <vector>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
};

#endif // GENERAL_INCLUDES_H_