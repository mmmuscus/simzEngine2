#include "../include/imGuiInstance.h"

void imGuiInstance::showGui(scene* currScene) {
    // ImGui::ShowDemoWindow();
    ImGui::Text("Object settings:");
    ImGui::BeginChild("Scrolling");
    for (size_t i = 0; i < currScene->getObjects().size(); i++) {
        showObjectGui(currScene->getObjects()[i]);
    }
    ImGui::EndChild();
}

void imGuiInstance::showObjectGui(object* obj) {
    ImGui::PushID(obj);

    ImGui::Text("Object %d:", obj->getObjectNumber());

    float pos[3] = { obj->getPos().x, obj->getPos().y, obj->getPos().z };
    float rot[3] = { obj->getRotation().x, obj->getRotation().y, obj->getRotation().z };
    float scale[3] = { obj->getScale().x, obj->getScale().y, obj->getScale().z };

    if (ImGui::DragFloat3("Position", pos, 0.1f))
        obj->setPos(glm::vec3(pos[0], pos[1], pos[2]));
    if (ImGui::DragFloat3("Rotation", rot, 0.1f))
        obj->setRotation(glm::vec3(rot[0], rot[1], rot[2]));
    if (ImGui::DragFloat3("Scale", scale, 0.1f))
        obj->setScale(glm::vec3(scale[0], scale[1], scale[2]));

    ImGui::PopID();
}

