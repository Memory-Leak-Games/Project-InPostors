#include "Player.h"

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "SceneGraph/Transform.h"

#include "CarMovementComponent.h"

Player::Player(const std::string& name, bool isStatic, mlg::Transform* parent)
: mlg::Entity(name, isStatic, parent) {}

std::shared_ptr<Player> Player::Create(const std::string& name, bool isStatic, mlg::Transform* parent) {
    auto newPlayer = std::shared_ptr<Player>(new Player(name, isStatic, parent));

    auto rigidbodyComponent = newPlayer->AddComponent<mlg::RigidbodyComponent>("Rigidbody");

    rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f, -0.5f), 0.5f);
    rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(glm::vec2(0.f, 0.5f), 0.5f);

    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Cars/car_one.obj");
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/cyan_material.json");

    auto staticMeshComponent = newPlayer->AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);
    newPlayer->AddComponent<CarMovementComponent>("MovementComponent");

    staticMeshComponent.lock()->GetTransform().SetPosition({0.f, -0.2f, 0.f});

    return newPlayer;
}
