#include "UI/PlayerUI.h"

#include "Core/TimerManager.h"
#include "Gameplay/Entity.h"
#include "Macros.h"
#include "Player/EquipmentComponent.h"
#include "Player/Player.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Builders/LabelBuilder.h"
#include "UI/Components/Containers/CanvasPanel.h"
#include "UI/Components/Containers/HorizontalBox.h"
#include "UI/Components/Image.h"
#include "UI/Components/Label.h"
#include "Utils/ProductManager.h"

PlayerUI::PlayerUI(const std::weak_ptr<mlg::Entity>& owner, const std::string& name)
    : Component(owner, name) {

    player = std::dynamic_pointer_cast<Player>(owner.lock());
    MLG_ASSERT_MSG(player != nullptr, "PlayerUI can only be added to Player");

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/player/arrow_material.json");

    // Arrow
    uiArrow = player->AddComponent<mlg::Image>("Arrow", material);
    auto ui = uiArrow.lock();
    ui->SetBillboardTarget(player);
    ui->SetSize({16.f, 16.f});
    ui->SetRelativePosition({0.f, 24.f});
    ui->tint = player->playerData.color;

    startLabel = player->AddComponent<mlg::Label>("PlayerStartLabel").lock();
    startLabel->SetBillboardTarget(owner);
    startLabel->SetSize(24);
    startLabel->SetTextColor(player->playerData.color);
    startLabel->SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center);
    startLabel->SetRelativePosition({-48.f, 40.f});
    startLabel->SetText(fmt::format("Player {:d}", player->playerData.id + 1));

    startTimerHandle = mlg::TimerManager::Get()->SetTimer(
            7.f,
            false,
            [this]() {
                this->startLabel->SetVisible(false);
            });

    // EqBillboard
    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/wood_material.json");

    for (auto& eqBillboard : eqBillboards) {
        eqBillboard = player->AddComponent<mlg::Image>("eqBillboard", material).lock();
        eqBillboard->SetBillboardTarget(player);
        eqBillboard->SetSize({14.f, 14.f});
        eqBillboard->SetVisible(false);
    }

    // Eq
    int equipmentSize = player->equipment->GetMaxSize();

    eqPanel = player->AddComponent<mlg::CanvasPanel>("EqPanel").lock();

    if (player->playerData.id == 0) {
        eqPanel.lock()->SetRelativePosition({100, 100});
    } else {
        eqPanel.lock()->SetRelativePosition({100, 50});
    }

    auto panelMaterial =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    "res/materials/ui/player/panel_material.json");
    ui = player->AddComponent<mlg::Image>("Panel", panelMaterial).lock();
    ui->SetSize({70.f + 36.f * equipmentSize, 38.f});
    ui->tint = player->playerData.color * 0.5f;
    ui->tint.a = 0.85f;
    eqPanel.lock()->AddChild(ui);

    auto iconsHbox = player->AddComponent<mlg::HorizontalBox>("IconsHbox").lock();
    eqPanel.lock()->AddChild(iconsHbox);

    mlg::LabelBuilder labelBuilder;
    labelBuilder.SetSize(32.f)
            .SetTextColor(player->playerData.color)
            .SetHorizontalAlignment(mlg::Label::HorizontalAlignment::Center)
            .SetVerticalAlignment(mlg::Label::VerticalAlignment::Center)
            .SetName("PlayerName");

    if (player->playerData.id == 0) {
        labelBuilder.SetText("P1");
    } else {
        labelBuilder.SetText("P2");
    }

    auto playerNameLabel = labelBuilder.BuildLabel(owner.lock().get());
    iconsHbox->AddChild(playerNameLabel);

    auto iconMaterial = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
            "res/materials/ui/icon/none_material.json");

    for (int i = 0; i < equipmentSize; ++i) {
        auto icon = player->AddComponent<mlg::Image>(
                                  "eqIcon", iconMaterial)
                            .lock();
        icon->SetSize({32.f, 32.f});
        iconsHbox->AddChild(icon);
        eqIcons.push_back(icon);
    }

    // Update equipment ui callback
    player->equipment->equipmentChanged.append(
            [this, equipmentSize]() {
                auto noneMaterial =
                        mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                                "res/materials/ui/icon/none_material.json");

                auto productManager = ProductManager::Get();
                auto items = this->player->equipment->GetEquipment();

                for (int i = 0; i < equipmentSize; ++i) {
                    if (!eqIcons[i] || !eqBillboards[i])
                        break;

                    if (items.size() > i) {
                        eqIcons[i]->SetMaterial(productManager->GetProduct(items[i]).icon);
                        eqBillboards[i]->SetMaterial(productManager->GetProduct(items[i]).icon);
                        eqIcons[i]->SetVisible(true);
                        eqBillboards[i]->SetVisible(true);
                    } else {
                        eqIcons[i]->SetMaterial(noneMaterial);
                        eqBillboards[i]->SetVisible(false);
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
