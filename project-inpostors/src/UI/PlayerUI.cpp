#include "UI/PlayerUI.h"

#include "Gameplay/Entity.h"
#include "Player/Player.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "Player/EquipmentComponent.h"
#include "Utils/ProductManager.h"

PlayerUI::PlayerUI(const std::weak_ptr<mlg::Entity>& owner, const std::string& name, const std::shared_ptr<class Player>& player)
    : Component(owner, name), player(player) {

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/player/arrow_material.json");

    uiArrow = player->AddComponent<mlg::Image>("Arrow", material);
    auto ui = uiArrow.lock();
    ui->SetBillboardTarget(player);
    ui->SetSize({16.f, 16.f});
    ui->SetRelativePosition({0.f, 24.f});
    ui->tint = player->playerData.color;

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/wood_material.json");

    for (auto& eqBillboard : eqBillboards) {
        eqBillboard = player->AddComponent<mlg::Image>("eqBillboard", material).lock();
        eqBillboard->SetBillboardTarget(player);
        eqBillboard->SetSize({14.f, 14.f});
        eqBillboard->SetVisible(false);
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/player/panel_material.json");
    ui = player->AddComponent<mlg::Image>("Panel", material).lock();
    ui->SetSize({70.f + 36.f * 3, 38.f});//TODO: Scale panel according to amount of slots in equipment
    ui->tint = player->playerData.color;
    ui->tint.a = 0.85f;
    if (player->playerData.id == 0) {
        ui->SetRelativePosition(ui->GetSize() * 0.5f + glm::vec2(8, 8));
    } else {
        ui->SetRelativePosition({1280 - ui->GetSize().x * 0.5f - 8, ui->GetSize().y * 0.5 + 8});
        ui->SetAnchor({1, 0});
    }
    ui->tint -= glm::vec4(0.8, 0.8, 0.8, 0.0);

    auto label = player->AddComponent<mlg::Label>("PlayerName").lock();
    label->SetSize(32);
    label->SetTextColor(player->playerData.color);
    label->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
    label->SetVerticalAlignment(mlg::Label::VerticalAlignment::Center);
    if (player->playerData.id == 0) {
        label->SetRelativePosition({8 + 25, 8 + 16});
        label->SetText("P1");
    } else {
        label->SetAnchor({1, 0});
        label->SetRelativePosition({1280 - 8 - 25, 8 + 16});
        label->SetText("P2");
    }

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/none_material.json");
    for (auto& eqIcon : eqIcons) {
        eqIcon = player->AddComponent<mlg::Image>("eqIcon", material).lock();
        eqIcon->SetSize({32.f, 32.f});
    }

    if (player->playerData.id == 0) {
        eqIcons[0]->SetRelativePosition({72.f + 8, 17.f + 8 + 2});
        eqIcons[1]->SetRelativePosition({72.f + 36.f + 8, 17.f + 8 + 2});
        eqIcons[2]->SetRelativePosition({72.f + 72.f + 8, 17.f + 8 + 2});
    } else {
        eqIcons[0]->SetRelativePosition({1280 - 72.f - 8, 17.f + 8 + 2});
        eqIcons[1]->SetRelativePosition({1280 - 72.f - 36.f - 8, 17.f + 8 + 2});
        eqIcons[2]->SetRelativePosition({1280 - 72.f - 72.f - 8, 17.f + 8 + 2});
        for (const auto& eqIcon : eqIcons)
            eqIcon->SetAnchor({1, 0});
    }

    // Update equipment ui callback
    player->equipment->equipmentChanged.append([this]() {
        auto noneMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/none_material.json");
        auto productManager = ProductManager::Get();
        auto items = this->player->equipment->GetEquipment();

        for (int i = 0; i < 3; ++i) {

            // Wouldn't game crash be more appropriate?
            if (!eqIcons[i] || !eqBillboards[i])
                break;

            if (items.size() > i) {
                eqIcons[i]->material = productManager->GetProduct(items[i]).icon;
                eqBillboards[i]->material = productManager->GetProduct(items[i]).icon;
                eqIcons[i]->SetVisible(true);
                eqBillboards[i]->SetVisible(true);
            } else {
                eqIcons[i]->material = noneMaterial;
                eqBillboards[i]->SetVisible(false);
                break;// It ain't much, but it's honest optimization
            }
        }

        // Discount HBoxContainer
        glm::vec2 billboardPos = {-7.f * (items.size() - 1.f), 40.f};
        for (int i = 0; i < items.size(); ++i) {
            eqBillboards[i]->SetRelativePosition(billboardPos);
            billboardPos.x += 14.f;
        }
    });
}

void PlayerUI::Start() {
}

void PlayerUI::Update() {
}
