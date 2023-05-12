#include "Buildings/Factory.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "SceneGraph/Transform.h"

using json = nlohmann::json;

Factory::Factory(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent)
: Entity(id, name, isStatic, parent) {}

std::shared_ptr<Factory> Factory::Create(uint64_t id, const std::string& name, bool isStatic,
                                         mlg::Transform* parent, const std::string& configPath) {
    auto result = std::shared_ptr<Factory>(new Factory(id, name, isStatic, parent));

    std::ifstream configFile{configPath};
    json configJson = json::parse(configFile);

    result->AddMesh(configJson["staticMesh"]);
    auto mainRigidbody = result->AddComponent<mlg::RigidbodyComponent>("MainRigidbody").lock();

    for (const auto& colliderJson: configJson["colliders"]) {
        result->AddCollider(colliderJson, mainRigidbody.get());
    }

    mainRigidbody->SetKinematic(true);
    return result;
}

void Factory::AddMesh(const json& staticMeshJson) {
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(staticMeshJson["model"].get<std::string>());
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(staticMeshJson["material"].get<std::string>());

    auto staticMeshComponent = AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);
    staticMeshComponent.lock()->GetTransform().SetPosition({
        staticMeshJson["position"][0].get<float>(),
        staticMeshJson["position"][1].get<float>(),
        staticMeshJson["position"][2].get<float>(),
    });

    staticMeshComponent.lock()->GetTransform().SetRotation(glm::radians(glm::vec3{
       staticMeshJson["rotation"][0].get<float>(),
       staticMeshJson["rotation"][1].get<float>(),
       staticMeshJson["rotation"][2].get<float>(),
    }));

    staticMeshComponent.lock()->GetTransform().SetRotation(glm::radians(glm::vec3{
            staticMeshJson["scale"].get<float>()
    }));
}

void Factory::AddCollider(const json& colliderJson, mlg::RigidbodyComponent* rigidbodyComponent) {
    glm::vec2 offset {
        colliderJson["offset"][0].get<float>(),
        colliderJson["offset"][1].get<float>()
    };

    glm::vec2 size {
        colliderJson["size"][0].get<float>(),
        colliderJson["size"][1].get<float>()
    };

    rigidbodyComponent->AddCollider<mlg::ColliderShape::Rectangle>(offset, size);
}
