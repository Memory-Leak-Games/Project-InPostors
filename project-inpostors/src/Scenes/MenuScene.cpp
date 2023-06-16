#include "Scenes/MenuScene.h"

#include "Core/AssetManager/AssetManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "SceneGraph/SceneGraph.h"

#include "UI/Assets/FontAsset.h"
#include "UI/Components/Image.h"
#include "UI/Components/Button.h"
#include "UI/Components/Label.h"
#include "UI/Components/UIComponent.h"
#include "UI/Components/VerticalBox.h"

#include "UI/UIStyle.h"

MenuScene::~MenuScene() = default;

void MenuScene::Load() {
    auto entity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
            "Image", false, mlg::SceneGraph::GetRoot());

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    "res/materials/ui/cat_ui_material.json");

    auto container =
            entity.lock()->AddComponent<mlg::VerticalBox>("VBox");

    container.lock()->SetAnchor(MLG_ANCHOR_CENTER);
    container.lock()->SetPosition(MLG_POS_CENTER);

    for (int i = 0; i < 5; i++) {
        auto exitButton = entity.lock()->AddComponent<mlg::Button>(
                fmt::format("Button {}", i),
                mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_MATERIAL),
                mlg::AssetManager::GetAsset<mlg::MaterialAsset>(BUTTON_FOCUSED_MATERIAL),
                mlg::AssetManager::GetAsset<mlg::FontAsset>(FONT));
        auto sharedExitButton = exitButton.lock();
        sharedExitButton->SetSize(BUTTON_SIZE);
        sharedExitButton->GetLabel().lock()->SetTextColor(glm::vec3(0.f));
        sharedExitButton->GetLabel().lock()->SetText(fmt::format("Button {}", i));
        sharedExitButton->SetPadding(10.f);

        container.lock()->AddChild(exitButton);
    }

    container.lock()->GrabFocus();
}
