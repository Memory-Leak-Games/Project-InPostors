#include "Player.h"

#include <memory>
#include <nlohmann/json.hpp>
#include <fstream>

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

#include "Gameplay/Entity.h"
#include "Rendering/Assets/ModelAsset.h"
#include "Rendering/Assets/MaterialAsset.h"

#include "SceneGraph/Transform.h"

#include "Car/CarMovementComponent.h"
#include "Gameplay/Components/ParticleSystemComponent.h"

#include "FX/SmokeFX.h"
#include "Car/PlayerTwoInput.h"

#include "Physics/Colliders/Collider.h"

#include "FX/FXLibrary.h"

#include "Utils/EquipmentComponent.h"
#include "Utils/ProductManager.h"

#include "Buildings/Factory.h"

using json = nlohmann::json;

Player::Player(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent, const PlayerData& playerData)
        : mlg::Entity(id, name, isStatic, parent), playerData(playerData) {}

std::shared_ptr<Player> Player::Create(uint64_t id, const std::string& name, bool isStatic,
                                       mlg::Transform* parent, const PlayerData& playerData) {
    auto newPlayer = std::shared_ptr<Player>(new Player(id, name, isStatic, parent, playerData));
    newPlayer->GetTransform().SetPosition(playerData.initialPosition);
  
    std::ifstream configFile{playerData.carData};
    json configJson = json::parse(configFile);

    newPlayer->AddRigidbody(configJson, playerData.initialRotation);
    newPlayer->LoadModel(configJson);

    newPlayer->AddComponent<CarMovementComponent>("MovementComponent", playerData.carData);
    newPlayer->equipment = newPlayer->AddComponent<EquipmentComponent>("EquipmentComponent", 3).lock();

    auto smoke = FXLibrary::Get("smoke");
    auto smokeComponent = newPlayer->AddComponent<mlg::ParticleSystemComponent>("SmokeFX", smoke);

    return newPlayer;
}

  
void Player::AddRigidbody(const json& configJson, float rotation = 0.f) {
    this->rigidbodyComponent = this->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
    this->rigidbodyComponent.lock()->SetBounciness(0.5f);
    this->rigidbodyComponent.lock()->SetRotation(rotation);
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

#ifdef DEBUG
    ImGui::Begin(("Player " + std::to_string(playerData.id)).c_str());
    ImGui::Text("%s", equipment->ToString().c_str());
    ImGui::End();
#endif
}

void Player::PickUp() {
    if (equipment->CheckIsFull())
        return;

    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    for (const auto& collider: overlappingColliders) {
        if (collider.lock()->GetTag() != "output" && collider.lock()->GetTag() != "inputOutput") {
            continue;
        }

        std::shared_ptr<mlg::Entity> owner = mlg::RigidbodyComponent::GetColliderOwner(*collider.lock()).lock();
        std::shared_ptr<Factory> factory = std::dynamic_pointer_cast<Factory>(owner);

        if (!factory)
            continue;

        const std::string product = factory->GetEquipmentComponent()->RequestProduct();

        if (product == "none")
            return;
        
        equipment->AddProduct(product);

        SPDLOG_WARN("{} : PickUp {} from {}", GetName(), product, factory->GetName());
    }

}
