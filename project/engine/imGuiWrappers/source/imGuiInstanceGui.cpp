#include "../include/imGuiInstance.h"

void imGuiInstance::showGui(
    scene* currScene,
    vulkanInstance* instance, vulkanPipeline* pipeline,
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
            instance, pipeline,
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
    if (ImGui::BeginCombo("Pipelines", pipelineManager->getVulkanPipelines()[currentVulkanPipelineItem]->getName().c_str())) {
        for (size_t i = 0; i < pipelineManager->getVulkanPipelines().size(); i++) {
            bool isSelected = i == currentVulkanPipelineItem;
            if (ImGui::Selectable(pipelineManager->getVulkanPipelines()[i]->getName().c_str()))
                currentVulkanPipelineItem = i;
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
            pipelineManager->getVulkanPipelines()[currentVulkanPipelineItem]->getName().c_str()
        );
        currScene->addObject(new gameObject(
            instance->getDevicePtr(), pipelineManager->getVulkanPipelines()[currentVulkanPipelineItem],
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
    transformComponent* transform = obj->getTransform();

    ImGui::PushID(obj);

    ImGui::Text("Object %d:", obj->getObjectNumber());

    float pos[3] = { transform->getPos().x, transform->getPos().y, transform->getPos().z };
    if (ImGui::DragFloat3("Position", pos, 0.1f))
        transform->setPos(glm::vec3(pos[0], pos[1], pos[2]));

    float quat[4] = { transform->getQuaternion().x, transform->getQuaternion().y, transform->getQuaternion().z, transform->getQuaternion().w};
    if (ImGui::DragFloat4("Quaternions", quat, 0.01f)) {
        transform->setQuaternion(Quat(quat[0], quat[1], quat[2], quat[3]));
    }

    glm::vec3 glmRot = transform->getEulerWithFlag();
    float qRot[3] = { glmRot.x, glmRot.y, glmRot.z };
    if (ImGui::DragFloat3("Rotation", qRot, 0.01f)) {
        transform->setQuaternion(Quat::fromEuler(glm::vec3(qRot[0], qRot[1], qRot[2])));
        transform->setOutsideRange(qRot[1]);
    }

    float scale[3] = { transform->getScale().x, transform->getScale().y, transform->getScale().z };
    if (ImGui::DragFloat3("Scale", scale, 0.1f))
        transform->setScale(glm::vec3(scale[0], scale[1], scale[2]));

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
                    obj->getVulkanPipeline()->getModelDescriptorSetLayout(),
                    obj->getVulkanPipeline()->getModelDescriptorPool()
                );
            }
        }

        ImGui::EndCombo();
    }
    // Pipeline
    if (ImGui::BeginCombo("Pipeline", obj->getVulkanPipeline()->getName().c_str())) {
        for (size_t i = 0; i < pipelineManager->getVulkanPipelines().size(); i++)
            if (ImGui::Selectable(pipelineManager->getVulkanPipelines()[i]->getName().c_str()))
                obj->setVulkanPipeline(pipelineManager->getVulkanPipelines()[i]);

        ImGui::EndCombo();
    }

    ImGui::PopID();
}
