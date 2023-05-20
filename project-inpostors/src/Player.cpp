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

#include "FX/FXLibrary.h"
#include "Utils/Equipment.h"

using json = nlohmann::json;

Player::Player(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent, const PlayerData& playerData)
        : mlg::Entity(id, name, isStatic, parent), playerData(playerData) {}

std::shared_ptr<Player> Player::Create(uint64_t id, const std::string& name, bool isStatic,
                                       mlg::Transform* parent, const PlayerData& playerData,
                                       const std::string& configPath) {
    auto newPlayer = std::shared_ptr<Player>(new Player(id, name, isStatic, parent, playerData));

    std::ifstream configFile{configPath};
    json configJson = json::parse(configFile);

    newPlayer->AddRigidbody(configJson);
    newPlayer->LoadModel(configJson);

    newPlayer->AddComponent<CarMovementComponent>("MovementComponent", configPath);
    newPlayer->AddComponent<Equipment>("Equipment", 3);

    auto smoke = FXLibrary::Get("smoke");
    auto smokeComponent = newPlayer->AddComponent<mlg::ParticleSystemComponent>("SmokeFX", smoke);

    return newPlayer;
}

void Player::AddRigidbody(const json& configJson) {
    this->rigidbodyComponent = this->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
    this->rigidbodyComponent.lock()->SetBounciness(0.5f);
    for (const auto& collider: configJson["colliders"]) {
        const glm::vec2 offset{collider["offset"][0].get<float>(), collider["offset"][1].get<float>(),};
        const float size = collider["size"].get<float>();

        this->rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(offset, size);
    }
}

void Player::LoadModel(const json& configJson) {
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(configJson["material"].get<std::string>());
    material = material->CreateDynamicInstance();
    material->SetVec4("color", playerData.color);

    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(configJson["model"].get<std::string>());
    auto staticMeshComponent = this->AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);
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
