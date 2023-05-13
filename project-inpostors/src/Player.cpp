#include "Player.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "SceneGraph/Transform.h"

#include "Car/CarMovementComponent.h"
#include "Gameplay/Components/ParticleSystemComponent.h"

#include "FX/SmokeFX.h"
#include "Car/PlayerTwoInput.h"

#include "Physics/Colliders/Collider.h"

using json = nlohmann::json;

Player::Player(uint64_t id, const std::string &name, bool isStatic, mlg::Transform *parent)
        : mlg::Entity(id, name, isStatic, parent) {}

std::shared_ptr<Player> Player::Create(uint64_t id, const std::string &name, bool isStatic,
                                       mlg::Transform *parent, const PlayerData &playerData,
                                       const std::string &configPath) {
    auto newPlayer = std::shared_ptr<Player>(new Player(id, name, isStatic, parent));

    newPlayer->rigidbodyComponent = newPlayer->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
    newPlayer->rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f, -0.5f), 0.5f);
    newPlayer->rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f, 0.5f), 0.5f);
    newPlayer->rigidbodyComponent.lock()->SetBounciness(0.5f);

    std::ifstream configFile{configPath};
    json configJson = json::parse(configFile);
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(configJson["model"].get<std::string>());
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(configJson["material"].get<std::string>());
    material = material->CreateDynamicInstance();

    material->SetVec4("color", playerData.color);

    auto staticMeshComponent = newPlayer->AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);
    newPlayer->AddComponent<CarMovementComponent>("MovementComponent");

    staticMeshComponent.lock()->GetTransform().SetPosition({0.f, 0.3f, 0.f});

    newPlayer->AddComponent<CarMovementComponent>("MovementComponent", configPath);

    auto smoke = std::make_shared<SmokeFX>();
    auto smokeComponent = newPlayer->AddComponent<mlg::ParticleSystemComponent>("SmokeFX", smoke);

    return newPlayer;
}

void Player::Start() {
    carInput = GetComponentByClass<CarInput>().lock();
}

void Player::Update() {
    if (carInput->GetPickUpInput())
        PickUp();

    if (carInput->GetDropInput()) {
        SPDLOG_WARN("{} : Drop", GetName());
    }
}

void Player::PickUp() {
    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    for (const auto& collider: overlappingColliders) {
        if (collider.lock()->GetTag() != "output" && collider.lock()->GetTag() != "inputOutput") {
            continue;
        }

        std::shared_ptr<Entity> factory = mlg::RigidbodyComponent::GetColliderOwner(*collider.lock()).lock();
        SPDLOG_WARN("{} : PickUp from {}", GetName(), factory->GetName());
    }

}
