#include "SimplePlayer.h"

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "SceneGraph/Transform.h"

#include "SimpleCarMovementComponent.h"

SimplePlayer::SimplePlayer(const std::string& name, bool isStatic, mlg::Transform* parent)
: mlg::Entity(name, isStatic, parent) {}

std::shared_ptr<SimplePlayer> SimplePlayer::Create(const std::string& name, bool isStatic, mlg::Transform* parent) {
    auto newPlayer = std::shared_ptr<SimplePlayer>(new SimplePlayer(name, isStatic, parent));

    newPlayer->AddComponent<mlg::RigidbodyComponent>("Rigidbody");

    auto model = mlg::AssetManager::GetAsset<mlg::ModelAsset>("res/models/Primitives/Sphere.obj");
    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/models/Primitives/blue_material.json");

    newPlayer->AddComponent<mlg::StaticMeshComponent>("StaticMeshComponent", model, material);
    newPlayer->AddComponent<SimpleCarMovementComponent>("MovementComponent");

    return newPlayer;
}
