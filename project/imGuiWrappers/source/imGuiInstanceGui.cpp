#include "../include/imGuiInstance.h"

void imGuiInstance::showGui(scene* currScene) {
    showObjectGui(currScene->getObjects()[0]);
}

void imGuiInstance::showObjectGui(object* obj) {
    bool changedPos = false;
    bool changedRot = false;
    bool changedScale = false;

    float pos[3] = { obj->getPos().x, obj->getPos().y, obj->getPos().z };
    changedPos = ImGui::InputFloat3("Position", pos);

    float rot[3] = { obj->getRotation().x, obj->getRotation().y, obj->getRotation().z };
    changedRot = ImGui::SliderFloat3("Rotation", rot, -180.0f, 180.0f);

    float scale[3] = { obj->getScale().x, obj->getScale().y, obj->getScale().z };
    changedScale = ImGui::InputFloat3("Scale", scale);

    if (changedPos) {
        obj->setPos(glm::vec3(pos[0], pos[1], pos[2]));
    }

    if (changedRot) {
        obj->setRotation(glm::vec3(rot[0], rot[1], rot[2]));
    }

    if (changedScale) {
        obj->setScale(glm::vec3(scale[0], scale[1], scale[2]));
    }
}

