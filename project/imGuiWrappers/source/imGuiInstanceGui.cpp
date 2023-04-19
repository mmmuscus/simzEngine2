#include "../include/imGuiInstance.h"

void imGuiInstance::drawGui(vk::CommandBuffer commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}
