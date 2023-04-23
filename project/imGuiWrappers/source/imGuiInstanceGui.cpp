#include "../include/imGuiInstance.h"

int currentMeshItem = 0;
int currentTextureItem = 0;

bool stdStringVectorItemGetter(void* data, int index, const char** out) {
    
    std::string* names = (std::string*)data;
    std::string& currentName = names[index];

    *out = names[index].c_str();

    return true;
}

void imGuiInstance::showGui(
    scene* currScene,
    vulkanInstance* instance, vulkanObject* obj,
    meshDataManager* meshManager, textureDataManager* textureManager,
    vulkanDynamicUniformBuffer* buffer, vulkanTextureSampler* sampler
) {
    ImGui::ShowDemoWindow();

    // Adding new objects by path:
    ImGui::PushID("Files");
    ImGui::Text("Add with files:");
    ImGui::InputText("Mesh Path", meshPath, IM_ARRAYSIZE(meshPath));
    ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));
    if (ImGui::Button("Add Object!")) {
        std::string currentMeshPath(meshPath);
        std::string currentTexturePath(texturePath);
        printf("Adding object: %s, %s\n", currentMeshPath.c_str(), currentTexturePath.c_str());
        currScene->addObject(new object(
            instance, obj,
            meshManager, currentMeshPath, buffer,
            textureManager, currentTexturePath, sampler
        ));
    }
    ImGui::PopID();

    // Adding new objects with resources:
    ImGui::PushID("Resources");
    ImGui::Text("Add with resources:");
    std::vector<std::string> meshNames;
    for (size_t i = 0; i < meshManager->getMeshDatas().size(); i++) 
        meshNames.push_back(meshManager->getMeshDatas()[i]->getName());
    ImGui::Combo(
        "Meshes", &currentMeshItem, 
        stdStringVectorItemGetter, meshNames.data(), meshNames.size()
    );
    std::vector<std::string> textureNames;
    for (size_t i = 0; i < textureManager->getTextureDatas().size(); i++)
        textureNames.push_back(textureManager->getTextureDatas()[i]->getName());
    ImGui::Combo(
        "Textures", &currentTextureItem,
        stdStringVectorItemGetter, textureNames.data(), textureNames.size()
    );
    if (ImGui::Button("Add Object!")) {
        printf("Adding object: %s, %s\n", meshNames[currentMeshItem].c_str(), textureNames[currentTextureItem].c_str());
        currScene->addObject(new object(
            instance->getDevice(), obj,
            meshManager, currentMeshItem,
            textureManager, currentTextureItem
        ));
    }
    ImGui::PopID();

    // Object settings
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

