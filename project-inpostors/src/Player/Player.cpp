#include "Player/Player.h"

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "Buildings/InteractiveBuilding.h"
#include "Core/Settings/SettingsManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/TimerManager.h"

#include "Gameplay/Entity.h"
#include "Player/PlayerFXComponent.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "SceneGraph/Transform.h"

#include "Car/CarMovementComponent.h"
#include "Gameplay/Components/ParticleSystemComponent.h"

#include "Car/PlayerTwoInput.h"
#include "FX/SmokeFX.h"

#include "Physics/Colliders/Collider.h"

#include "FX/FXLibrary.h"

#include "Player/EquipmentComponent.h"
#include "Utils/BlueprintManager.h"
#include "Utils/ProductManager.h"

#include "Buildings/Factory.h"

#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"

#include "UI/PlayerUI.h"

using json = nlohmann::json;

Player::Player(
        uint64_t id,
        const std::string& name,
        bool isStatic,
        mlg::Transform* parent,
        const PlayerData& playerData)
    : mlg::Entity(id, name, isStatic, parent), playerData(playerData) {}

std::shared_ptr<Player> Player::Create(
        uint64_t id, const std::string& name, bool isStatic,
        mlg::Transform* parent, const PlayerData& playerData) {

    auto newPlayer = std::shared_ptr<Player>(
            new Player(id, name, isStatic, parent, playerData));

    std::ifstream configFile{playerData.carData};
    json configJson = json::parse(configFile);

    newPlayer->AddRigidbody(configJson);
    newPlayer->LoadModel(configJson);

    newPlayer->AddComponent<CarMovementComponent>(
            "MovementComponent", playerData.carData);

    int equipmentSize = configJson.value("equipmentSize", 1);
    newPlayer->equipment = newPlayer->AddComponent<EquipmentComponent>(
                                            "EquipmentComponent",
                                            equipmentSize)
                                   .lock();

    // newPlayer->AddComponent<PlayerUI>("PlayerUI");
    newPlayer->AddComponent<PlayerFXComponent>("PlayerFXComponent");

    return newPlayer;
}

void Player::AddRigidbody(const json& configJson) {
    this->rigidbodyComponent = this->AddComponent<mlg::RigidbodyComponent>("Rigidbody");

    float bounciness = configJson["parameters"].value("bounciness", 0.5f);
    this->rigidbodyComponent.lock()->SetBounciness(bounciness);

    for (const auto& collider : configJson["colliders"]) {
        const glm::vec2 offset{
                collider["offset"][0].get<float>(),
                collider["offset"][1].get<float>(),
        };
        const float size = collider["size"].get<float>();

        this->rigidbodyComponent.lock()->AddCollider<mlg::ColliderShape::Circle>(
                offset, size);
    }
}

void Player::LoadModel(const json& configJson) {
    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    configJson["material"]);

    material = material->CreateDynamicInstance();
    material->SetVec4("color", playerData.color);

    auto model =
            mlg::AssetManager::GetAsset<mlg::ModelAsset>(configJson["model"]);
    auto staticMeshComponent = this->AddComponent<mlg::StaticMeshComponent>(
            "StaticMeshComponent", model, material);
}

void Player::Start() {
    carInput = GetComponentByClass<CarInput>().lock();

    pickUpSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/pick_up.wav");
    dropSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/drop.wav");
    hitSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/hit.wav");
}

void Player::Update() {
    if (carInput->GetPickUpInput()) {
        if (!PickUp())
            Drop();
    }

    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    if (canPlaySound) {
        auto enableSoundLambda = [this]() {
            canPlaySound = true;
        };

        for (const auto& collider : overlappingColliders) {
            if (collider.lock()->GetTag().empty()) {
                hitSound->Play();
                canPlaySound = false;
                mlg::TimerManager::Get()->SetTimer(
                        0.3f, false, enableSoundLambda);
            }
        }
    }

#ifdef DEBUG
    if (mlg::SettingsManager::Get<bool>(
                mlg::SettingsType::Debug, "showPlayerEq")) {

        ImGui::Begin(("Player " + std::to_string(playerData.id)).c_str());
        ImGui::Text("%s", equipment->ToString().c_str());
        ImGui::End();
    }
#endif
}

void Player::SetPlayerPosition(const glm::vec2& position) {
    rigidbodyComponent.lock()->SetPosition(position);
}

void Player::SetPlayerRotation(float angle) {
    rigidbodyComponent.lock()->SetRotation(angle);
}

bool Player::PickUp() {
    if (equipment->IsFull())
        return false;

    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    const auto& output = std::find_if(
            overlappingColliders.begin(),
            overlappingColliders.end(),
            [](const auto& collider) {
                return collider.lock()->GetTag() == "output" ||
                       collider.lock()->GetTag() == "inputOutput";
            });

    if (output == overlappingColliders.end())
        return false;

    std::shared_ptr<mlg::Entity> owner =
            mlg::RigidbodyComponent::GetColliderOwner(*output->lock()).lock();
    std::shared_ptr<Factory> factory = std::dynamic_pointer_cast<Factory>(owner);

    if (!factory)
        return false;

    std::string outputProduct = factory->GiveOutput();

    if (outputProduct == "None")
        return false;

    equipment->AddProduct(outputProduct);
    pickUpSound->Play(4.f);

    return true;
}

bool Player::Drop() {
    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    const auto& input = std::find_if(
            overlappingColliders.begin(),
            overlappingColliders.end(),
            [](const auto& collider) {
                return collider.lock()->GetTag() == "input" ||
                       collider.lock()->GetTag() == "inputOutput";
            });

    if (input == overlappingColliders.end())
        return false;

    std::shared_ptr<mlg::Entity> owner =
            mlg::RigidbodyComponent::GetColliderOwner(*input->lock()).lock();
    auto factory = std::dynamic_pointer_cast<InteractiveBuilding>(owner);

    if (!factory)
        return false;

    if (!factory->TakeInputsFromInventory(*equipment))
        return false;

    dropSound->Play(4.f);
    return true;
}