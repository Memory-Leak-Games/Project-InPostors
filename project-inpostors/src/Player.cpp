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

using json = nlohmann::json;

Player::Player(const std::string &name, bool isStatic, mlg::Transform *parent)
        : mlg::Entity(name, isStatic, parent) {}

std::shared_ptr<Player> Player::Create(const std::string &name, bool isStatic, mlg::Transform *parent,
                                       const PlayerData& playerData,
                                       const std::string& configPath) {
    auto newPlayer = std::shared_ptr<Player>(new Player(name, isStatic, parent));

    auto rigidbodyComponent = newPlayer->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
    rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f, -0.5f), 0.5f);
    rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f, 0.5f), 0.5f);
    rigidbodyComponent.lock()->SetBounciness(0.5f);

    std::ifstream configFile{configPath};
    json configJson = json::parse(configFile);
    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>(configJson["model"].get<std::string>());
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(configJson["material"].get<std::string>());
    material = material->CreateDynamicInstance();

    material->SetVec4("color", playerData.color);

    auto staticMeshComponent = newPlayer->AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);
    staticMeshComponent.lock()->GetTransform().SetPosition({0.f, -0.2f, 0.f});

    newPlayer->AddComponent<CarMovementComponent>("MovementComponent", configPath);

    return newPlayer;
}
