#include "Buildings/InteractiveBuilding.h"

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Physics/Colliders/Collider.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "Gameplay/Components/ParticleSystemComponent.h"

#include "FX/FXLibrary.h"

InteractiveBuilding::InteractiveBuilding(
        size_t id,
        const std::string& name, bool isStatic,
        mlg::Transform* parent)
    : Entity(id, name, isStatic, parent) {}

void InteractiveBuilding::AddMesh(const nlohmann::json& staticMeshJson) {
    auto model =
            mlg::AssetManager::GetAsset<mlg::ModelAsset>(
                    staticMeshJson["model"]);
    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    staticMeshJson["material"].get<std::string>());

    auto staticMeshComponent =
            AddComponent<mlg::StaticMeshComponent>(
                    "StaticMeshComponent", model, material);

    staticMeshComponent.lock()->GetTransform().SetPosition({
            staticMeshJson["position"][0],
            staticMeshJson["position"][1],
            staticMeshJson["position"][2],
    });

    meshOffset = {staticMeshJson["position"][0],
                  staticMeshJson["position"][1]};

    staticMeshComponent.lock()->GetTransform().SetRotation(glm::radians(glm::vec3{
            0.f, staticMeshJson["rotation"], 0.f}));

    staticMeshComponent.lock()->GetTransform().SetScale(
            glm::vec3{staticMeshJson["scale"]});
}

void InteractiveBuilding::AddCollider(const nlohmann::json& colliderJson,
                                      mlg::RigidbodyComponent* rigidbodyComponent) {

    glm::vec2 offset{colliderJson["offset"][0], colliderJson["offset"][1]};
    glm::vec2 size{colliderJson["size"][0], colliderJson["size"][1]};

    rigidbodyComponent->AddCollider<mlg::ColliderShape::Rectangle>(offset, size);
}

void InteractiveBuilding::AddEmitter(const nlohmann::json& emitterJson) {
    const std::string id = emitterJson["id"].get<std::string>();
    std::shared_ptr<mlg::ParticleSystem> emitter = FXLibrary::Get(id);
    auto emitterComponent =
            AddComponent<mlg::ParticleSystemComponent>(id, emitter);

    glm::vec3 emitterPosition = {
            emitterJson["position"][0],
            emitterJson["position"][1],
            emitterJson["position"][2]};
    emitterPosition.x += meshOffset.x;
    emitterPosition.z += meshOffset.y;
    emitterComponent.lock()->GetTransform().SetPosition(emitterPosition);
}

void InteractiveBuilding::AddTrigger(const nlohmann::json& triggerJson,
                                     const std::string& triggerName,
                                     mlg::RigidbodyComponent* rigidbodyComponent) {
    glm::vec2 offset{triggerJson["offset"][0], triggerJson["offset"][1]};
    glm::vec2 size{triggerJson["size"][0], triggerJson["size"][1]};

    auto collider =
            rigidbodyComponent->AddTrigger<mlg::ColliderShape::Rectangle>(offset, size);

    collider.lock()->SetTag(triggerName);
}
