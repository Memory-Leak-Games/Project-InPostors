#include "Player.h"

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "Core/Settings/SettingsManager.h"
#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"
#include "Core/TimerManager.h"

#include "Gameplay/Entity.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/ModelAsset.h"

#include "SceneGraph/Transform.h"

#include "Car/CarMovementComponent.h"
#include "Gameplay/Components/ParticleSystemComponent.h"

#include "Car/PlayerTwoInput.h"
#include "FX/SmokeFX.h"

#include "Physics/Colliders/Collider.h"

#include "FX/FXLibrary.h"

#include "Utils/BlueprintManager.h"
#include "Utils/EquipmentComponent.h"
#include "Utils/ProductManager.h"

#include "Buildings/Factory.h"

#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"

using json = nlohmann::json;

Player::Player(uint64_t id, const std::string& name, bool isStatic, mlg::Transform* parent, const PlayerData& playerData)
    : mlg::Entity(id, name, isStatic, parent), playerData(playerData) {}

std::shared_ptr<Player> Player::Create(uint64_t id, const std::string& name, bool isStatic,
                                       mlg::Transform* parent, const PlayerData& playerData) {
    auto newPlayer = std::shared_ptr<Player>(new Player(id, name, isStatic, parent, playerData));

    std::ifstream configFile{playerData.carData};
    json configJson = json::parse(configFile);

    newPlayer->AddRigidbody(configJson);
    newPlayer->LoadModel(configJson);

    newPlayer->AddComponent<CarMovementComponent>("MovementComponent", playerData.carData);
    newPlayer->equipment = newPlayer->AddComponent<EquipmentComponent>("EquipmentComponent", 3).lock();

    auto smoke = FXLibrary::Get("smoke");
    auto smokeComponent = newPlayer->AddComponent<mlg::ParticleSystemComponent>("SmokeFX", smoke);

    GenerateUI(newPlayer);

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

    pickUpSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/pick_up.wav");
    dropSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/drop.wav");
    hitSound = mlg::AssetManager::GetAsset<mlg::AudioAsset>("res/audio/sfx/hit.wav");
}

void Player::Update() {
    if (carInput->GetPickUpInput())
        PickUp();

    if (carInput->GetDropInput())
        Drop();

    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    if (canPlaySound)
    {
        auto enableSoundLambda = [this]() {
            canPlaySound = true;
        };

        for (const auto& collider : overlappingColliders) {
            if (collider.lock()->GetTag().empty()) {
                hitSound->Play();
                canPlaySound = false;
                mlg::TimerManager::Get()->SetTimer(0.3f, false, enableSoundLambda);
            }
        }
    }

#ifdef DEBUG
    if (mlg::SettingsManager::Get<bool>(mlg::SettingsType::Debug, "showPlayerEq")) {
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

void Player::PickUp() {
    if (equipment->IsFull())
        return;

    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    for (const auto& collider : overlappingColliders) {
        if (collider.lock()->GetTag() != "output" && collider.lock()->GetTag() != "inputOutput") {
            continue;
        }

        std::shared_ptr<mlg::Entity> owner = mlg::RigidbodyComponent::GetColliderOwner(*collider.lock()).lock();
        std::shared_ptr<Factory> factory = std::dynamic_pointer_cast<Factory>(owner);

        if (!factory)
            continue;

        const std::string factoryOutput =
                BlueprintManager::Get()->GetBlueprint(factory->GetBlueprintId()).GetOutput();

        if (!factory->GetEquipmentComponent()->RequestProduct(factoryOutput))
            return;

        equipment->AddProduct(factoryOutput);
        pickUpSound->Play(4.f);
    }
}

void Player::Drop() {
    std::vector<std::weak_ptr<mlg::Collider>> overlappingColliders;
    rigidbodyComponent.lock()->GetOverlappingColliders(overlappingColliders);

    for (const auto& collider : overlappingColliders) {
        if (collider.lock()->GetTag() != "input" && collider.lock()->GetTag() != "inputOutput") {
            continue;
        }

        std::shared_ptr<mlg::Entity> owner = mlg::RigidbodyComponent::GetColliderOwner(*collider.lock()).lock();
        std::shared_ptr<Factory> factory = std::dynamic_pointer_cast<Factory>(owner);

        if (!factory)
            continue;

        const std::vector<std::string> factoryInputs = factory->GetInputs();

        for (const auto& item : factoryInputs) {
            if (!equipment->Has(item))
                continue;

            if (!factory->GetEquipmentComponent()->AddProduct(item))
                continue;

            equipment->RequestProduct(item);
            dropSound->Play(4.f);

            return;
        }
    }
}

void Player::GenerateUI(const std::shared_ptr<Player>& newPlayer) {

    auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/terminus-bold.ttf");

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/player/arrow_material.json");
    newPlayer->uiArrow = newPlayer->AddComponent<mlg::Image>("Arrow", material);
    auto ui = newPlayer->uiArrow.lock();
    ui->SetBillboardTarget(newPlayer);
    ui->SetSize({16.f, 16.f});
    ui->SetPosition({0.f, 24.f});
    ui->tint = newPlayer->playerData.color;

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/wood_material.json");
    for(int i = 0; i < 3; ++i) {
        newPlayer->eqBillboards[i] = newPlayer->AddComponent<mlg::Image>("eqBillboard", material).lock();
        newPlayer->eqBillboards[i]->SetBillboardTarget(newPlayer);
        newPlayer->eqBillboards[i]->SetSize({14.f, 14.f});
        newPlayer->eqBillboards[i]->SetVisible(false);
    }

//    auto label = newPlayer->AddComponent<mlg::Label>("PlayerTag", font).lock();
//    label->SetSize(12);
//    label->SetTextColor(newPlayer->playerData.color);
//    label->SetBillboardTarget(newPlayer);
//    label->SetPosition({-8.f, 35.f}); //TODO: Change when label's horizontal alignment as billboard is fixed
//    label->SetTextColor(newPlayer->playerData.color);
//    if(newPlayer->playerData.id == 0) {
//        label->SetText("P1");
//    } else {
//        label->SetText("P2");
//    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/player/panel_material.json");
    ui = newPlayer->AddComponent<mlg::Image>("Panel", material).lock();
    ui->SetSize({70.f + 36.f * 3, 38.f}); //TODO: Scale panel according to amout of slots in equipment
    ui->tint = newPlayer->playerData.color;
    ui->tint.a = 0.85f;
    if(newPlayer->playerData.id == 0)
    {
        ui->SetPosition(ui->GetSize() * 0.5f + glm::vec2(8, 8));
    } else {
        ui->SetPosition({1280 - ui->GetSize().x * 0.5f - 8, ui->GetSize().y * 0.5 + 8});
        ui->SetAnchor({1, 0});
    }
    ui->tint -= glm::vec4(0.8, 0.8, 0.8, 0.0);

    auto label = newPlayer->AddComponent<mlg::Label>("PlayerName").lock();
    label->SetSize(32);
    label->SetTextColor(newPlayer->playerData.color);
    label->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
    label->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
    if(newPlayer->playerData.id == 0)
    {
        label->SetPosition({8 + 25, 8 + 16});
        label->SetText("P1");
    } else {
        label->SetAnchor({1, 0});
        label->SetPosition({1280 - 8 - 25, 8 + 16});
        label->SetText("P2");
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/none_material.json");
    for(int i = 0; i < 3; ++i) {
        newPlayer->eqIcons[i] = newPlayer->AddComponent<mlg::Image>("eqIcon", material).lock();
        newPlayer->eqIcons[i]->SetSize({32.f, 32.f});
    }

    if(newPlayer->playerData.id == 0) {
        newPlayer->eqIcons[0]->SetPosition({72.f + 8, 17.f + 8 + 2});
        newPlayer->eqIcons[1]->SetPosition({72.f+36.f + 8, 17.f + 8 + 2});
        newPlayer->eqIcons[2]->SetPosition({72.f+72.f + 8, 17.f + 8 + 2});
    } else {
        newPlayer->eqIcons[0]->SetPosition({1280 - 72.f - 8, 17.f + 8 + 2});
        newPlayer->eqIcons[1]->SetPosition({1280 - 72.f - 36.f - 8, 17.f + 8 + 2});
        newPlayer->eqIcons[2]->SetPosition({1280 - 72.f - 72.f - 8, 17.f + 8 + 2});
        for(int i = 0; i < 3; ++i)
            newPlayer->eqIcons[i]->SetAnchor({1, 0});
    }

    // Update equipment ui callback
    newPlayer->equipment->equipmentChanged.append([newPlayer]() {
        auto noneMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/none_material.json");
        auto productManager = ProductManager::GetInstance();
        auto items = newPlayer->equipment->GetEquipment();

        for(int i = 0; i < 3; ++i) {

            // Wouldn't game crash be more appropriate?
            if (!newPlayer->eqIcons[i] || !newPlayer->eqBillboards[i])
                break;

            if(items.size() > i) {
                newPlayer->eqIcons[i]->material = productManager->GetProduct(items[i]).icon;
                newPlayer->eqBillboards[i]->material = productManager->GetProduct(items[i]).icon;
                newPlayer->eqIcons[i]->SetVisible(true);
                newPlayer->eqBillboards[i]->SetVisible(true);
            } else {
                newPlayer->eqIcons[i]->material = noneMaterial;
                newPlayer->eqBillboards[i]->SetVisible(false);
                break; // It ain't much, but it's honest optimization
            }
        }

        // Discount HBoxContainer
        glm::vec2 billboardPos = {-7.f * (items.size() - 1.f), 40.f};
        for(int i = 0; i < items.size(); ++i) {
            newPlayer->eqBillboards[i]->SetPosition(billboardPos);
            billboardPos.x += 14.f;
        }
    });
}
