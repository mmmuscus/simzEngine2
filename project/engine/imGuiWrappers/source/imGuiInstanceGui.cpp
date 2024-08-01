#include "../include/imGuiInstance.h"

void imGuiInstance::showGui(
    scene* currScene,
    vulkanInstance* instance, vulkanObject* obj,
    vulkanDynamicUniformBuffer* buffer, vulkanTextureSampler* sampler
) {
    // Adding new objects by path:
    ImGui::PushID("Files");
    ImGui::Text("Add with path:");
    ImGui::InputText("Mesh Path", meshPath, IM_ARRAYSIZE(meshPath));
    ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));
    if (ImGui::Button("Add Object!")) {
        std::string currentMeshPath(meshPath);
        std::string currentTexturePath(texturePath);
        printf("Adding object: %s, %s\n", currentMeshPath.c_str(), currentTexturePath.c_str());
        currScene->addObject(new gameObject(
            instance, obj,
            meshManager, currentMeshPath, buffer,
            textureManager, currentTexturePath, sampler
        ));
    }
    ImGui::PopID();

    // Adding new objects with resources:
    ImGui::PushID("Resources");
    ImGui::Text("Add with resources:");
    if (ImGui::BeginCombo("Meshes", meshManager->getMeshDatas()[currentMeshItem]->getName().c_str())) {
        for (size_t i = 0; i < meshManager->getMeshDatas().size(); i++) {
            bool isSelected = i == currentMeshItem;
            if (ImGui::Selectable(meshManager->getMeshDatas()[i]->getName().c_str(), isSelected))
                currentMeshItem = i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (ImGui::BeginCombo("Textures", textureManager->getTextureDatas()[currentTextureItem]->getName().c_str())) {
        for (size_t i = 0; i < textureManager->getTextureDatas().size(); i++) {
            bool isSelected = i == currentTextureItem;
            if (ImGui::Selectable(textureManager->getTextureDatas()[i]->getName().c_str(), isSelected))
                currentTextureItem = i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (ImGui::BeginCombo("Pipelines", objectManager->getVulkanObjects()[currentVulkanObjectItem]->getName().c_str())) {
        for (size_t i = 0; i < objectManager->getVulkanObjects().size(); i++) {
            bool isSelected = i == currentVulkanObjectItem;
            if (ImGui::Selectable(objectManager->getVulkanObjects()[i]->getName().c_str()))
                currentVulkanObjectItem = i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (ImGui::Button("Add Object!")) {
        printf(
            "Adding object: %s, %s, %s\n",
            meshManager->getMeshDatas()[currentMeshItem]->getName().c_str(),
            textureManager->getTextureDatas()[currentTextureItem]->getName().c_str(),
            objectManager->getVulkanObjects()[currentVulkanObjectItem]->getName().c_str()
        );
        currScene->addObject(new gameObject(
            instance->getDevicePtr(), objectManager->getVulkanObjects()[currentVulkanObjectItem],
            meshManager, currentMeshItem,
            textureManager, currentTextureItem
        ));
    }
    ImGui::PopID();

    // Object settings
    ImGui::Text("Object settings:");
    ImGui::BeginChild("Scrolling");
    for (size_t i = 0; i < currScene->getObjects().size(); i++) {
        showObjectEditGui(currScene->getObjects()[i]);
    }
    ImGui::EndChild();
}

void imGuiInstance::showObjectEditGui(gameObject* obj) {
    ImGui::PushID(obj);

    ImGui::Text("Object %d:", obj->getObjectNumber());

    float pos[3] = { obj->getPos().x, obj->getPos().y, obj->getPos().z };
    if (ImGui::DragFloat3("Position", pos, 0.1f))
        obj->setPos(glm::vec3(pos[0], pos[1], pos[2]));

    float quat[4] = {obj->getQuaternion().x, obj->getQuaternion().y, obj->getQuaternion().z, obj->getQuaternion().w};
    if (ImGui::DragFloat4("Quaternions", quat, 0.01f)) {
        Quat q = Quat(quat[0], quat[1], quat[2], quat[3]);
        obj->setQuaternion(q);
    }
    
    Quat tmpQ = obj->getQuaternion();
    glm::vec3 glmRot = Quat::toEuler(tmpQ);
    float qRot[3] = { glmRot.x, glmRot.y, glmRot.z };
    ImGui::Text( "Rotation from Quaternions: x: %f, y: %f, z: %f", qRot[0], qRot[1], qRot[2]);
    float rot[3] = { obj->getRotation().x, obj->getRotation().y, obj->getRotation().z };
    if (ImGui::DragFloat3("Rotation", rot, 0.01f)) {
        obj->setRotation(glm::vec3(rot[0], rot[1], rot[2]));
        //glmRot = glm::vec3(rot[0], rot[1], rot[2]);
        //tmpQ = Quat::fromEuler(glmRot);
        //obj->setQuaternion(tmpQ);
    }

    float scale[3] = { obj->getScale().x, obj->getScale().y, obj->getScale().z };
    if (ImGui::DragFloat3("Scale", scale, 0.1f))
        obj->setScale(glm::vec3(scale[0], scale[1], scale[2]));

    // Mesh
    if (ImGui::BeginCombo("Mesh", obj->getModelData()->getMeshData()->getName().c_str())) {
        for (size_t i = 0; i < meshManager->getMeshDatas().size(); i++)
            if (ImGui::Selectable(meshManager->getMeshDatas()[i]->getName().c_str()))
                obj->getModelData()->setMeshData(meshManager->getMeshDatas()[i]);

        ImGui::EndCombo();
    }
    // Texture
    if (ImGui::BeginCombo("Texture", obj->getModelData()->getTextureData()->getName().c_str())) {
        for (size_t i = 0; i < textureManager->getTextureDatas().size(); i++) {
            if (ImGui::Selectable(textureManager->getTextureDatas()[i]->getName().c_str())) {
                obj->getModelData()->setTextureData(textureManager->getTextureDatas()[i]);
                obj->getModelData()->initDescriptorSets(
                    obj->getVulkanObject()->getModelDescriptorSetLayout(),
                    obj->getVulkanObject()->getModelDescriptorPool()
                );
            }
        }

        ImGui::EndCombo();
    }
    // Pipeline
    if (ImGui::BeginCombo("Pipeline", obj->getVulkanObject()->getName().c_str())) {
        for (size_t i = 0; i < objectManager->getVulkanObjects().size(); i++) 
            if (ImGui::Selectable(objectManager->getVulkanObjects()[i]->getName().c_str()))
                obj->setVulkanObject(objectManager->getVulkanObjects()[i]);

        ImGui::EndCombo();
    }

    ImGui::PopID();
}
