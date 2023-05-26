#include "Player.h"

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "Gameplay/Components/RigidbodyComponent.h"
#include "Gameplay/Components/StaticMeshComponent.h"

#include "Core/AssetManager/AssetManager.h"

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
    newPlayer->GetTransform().SetPosition(playerData.initialPosition);

    std::ifstream configFile{playerData.carData};
    json configJson = json::parse(configFile);

    newPlayer->AddRigidbody(configJson, playerData.initialRotation);
    newPlayer->LoadModel(configJson);

    newPlayer->AddComponent<CarMovementComponent>("MovementComponent", playerData.carData);
    newPlayer->equipment = newPlayer->AddComponent<EquipmentComponent>("EquipmentComponent", 3).lock();

    auto smoke = FXLibrary::Get("smoke");
    auto smokeComponent = newPlayer->AddComponent<mlg::ParticleSystemComponent>("SmokeFX", smoke);

    GenerateUI(newPlayer);

    return newPlayer;
}

void Player::AddRigidbody(const json& configJson, float rotation = 0.f) {
    this->rigidbodyComponent = this->AddComponent<mlg::RigidbodyComponent>("Rigidbody");
    this->rigidbodyComponent.lock()->SetBounciness(0.5f);
    this->rigidbodyComponent.lock()->SetRotation(rotation);
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
    uiArrow.lock()->tint = playerData.color;
}

void Player::Update() {
    if (carInput->GetPickUpInput())
        PickUp();

    if (carInput->GetDropInput())
        Drop();

#ifdef DEBUG
    ImGui::Begin(("Player " + std::to_string(playerData.id)).c_str());
    ImGui::Text("%s", equipment->ToString().c_str());
    ImGui::End();
#endif
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

        // TODO: remove me
        SPDLOG_WARN("{} : PickUp {} from {}", GetName(), factoryOutput, factory->GetName());
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

        const std::vector<std::string> factoryInputs =
                BlueprintManager::Get()->GetBlueprint(factory->GetBlueprintId()).GetInput();

        for (const auto& item : factoryInputs) {
            if (!equipment->Has(item))
                continue;

            // TODO: this may cause problems when factory needs multiple types of inputs,
            //       but factory eq is full of garbage.

            if (!factory->GetEquipmentComponent()->AddProduct(item))
                continue;

            equipment->RequestProduct(item);

            // TODO: remove me
            SPDLOG_WARN("{} : Drop {} to {}", GetName(), item, factory->GetName());
            return;
        }
    }
}

void Player::GenerateUI(const std::shared_ptr<Player>& newPlayer) {
    auto font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/arialbd.ttf");

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/player/arrow_material.json");
    newPlayer->uiArrow = newPlayer->AddComponent<mlg::Image>("Arrow", material);
    auto ui = newPlayer->uiArrow.lock();
    ui->SetBillboardTarget(newPlayer);
    ui->SetSize({14.f, 14.f});
    ui->SetPosition({0.f, 24.f});
    ui->tint = newPlayer->playerData.color;

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/wood_material.json");
    ui = newPlayer->AddComponent<mlg::Image>("WoodEq", material).lock();
    ui->SetBillboardTarget(newPlayer);
    ui->SetSize({14.f, 14.f});
    ui->SetPosition({7.f, 53.f});

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/metal_material.json");
    ui = newPlayer->AddComponent<mlg::Image>("MetalEq", material).lock();
    ui->SetBillboardTarget(newPlayer);
    ui->SetSize({14.f, 14.f});
    ui->SetPosition({-7.f, 53.f});

    auto label = newPlayer->AddComponent<mlg::Label>("PlayerTag", font).lock();
    label->SetSize(12);
    label->SetTextColor(newPlayer->playerData.color);
    label->SetBillboardTarget(newPlayer);
    label->SetPosition({-6.f, 35.f});
    if(newPlayer->GetName() == "Player")
    {
        label->SetText("P1");
    } else {
        label->SetText("P2");
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/player/panel_material.json");
    ui = newPlayer->AddComponent<mlg::Image>("Panel", material).lock();
    ui->SetSize({200.f, 38.f});
    if(newPlayer->GetName() == "Player")
    {
        ui->SetPosition(ui->GetSize() * 0.5f);
    } else {
        ui->SetPosition({1280 - ui->GetSize().x * 0.5f, ui->GetSize().y * 0.5});
        ui->SetAnchor({1, 0});
    }

    label = newPlayer->AddComponent<mlg::Label>("PlayerName", font).lock();
    label->SetSize(32);
    label->SetTextColor(newPlayer->playerData.color);
    if(newPlayer->GetName() == "Player")
    {
        label->SetPosition({10, 8});
        label->SetText("P1");
    } else {
        label->SetAnchor({1, 0});
        label->SetPosition({1280 - 50, 8});
        label->SetText("P2");
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/wood_material.json");
    ui = newPlayer->AddComponent<mlg::Image>("WoodEq", material).lock();
    ui->SetSize({32.f, 32.f});
    if(newPlayer->GetName() == "Player")
    {
        ui->SetPosition({72.f, 17.f});
    } else {
        ui->SetPosition({1280 - 72.f, 17.f});
        ui->SetAnchor({1, 0});
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/metal_material.json");
    ui = newPlayer->AddComponent<mlg::Image>("MetalEq", material).lock();
    ui->SetSize({32.f, 32.f});
    if(newPlayer->GetName() == "Player")
    {
        ui->SetPosition({72.f+36.f, 17.f});
    } else {
        ui->SetPosition({1280 - 72.f - 36.f, 17.f});
        ui->SetAnchor({1, 0});
    }
}
