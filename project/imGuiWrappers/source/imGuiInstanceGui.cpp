#include "../include/imGuiInstance.h"

bool changed = false;
float test = 1.0f;

void imGuiInstance::showGui() {
    changed = ImGui::InputFloat("test:", &test);
    if (changed) {
        printf("%f\n", test);
        changed = false;
    }
}
