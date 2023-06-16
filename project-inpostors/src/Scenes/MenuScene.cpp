#include "Scenes/MenuScene.h"

#include "Core/AssetManager/AssetManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "SceneGraph/SceneGraph.h"

#include "UI/Components/Image.h"
#include "UI/Components/UIComponent.h"
#include "UI/Components/VerticalBox.h"
#include <glm/fwd.hpp>

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

    for (int i = 0; i < 10; i++) {
        auto image = entity.lock()->AddComponent<mlg::Image>(
                "Image",
                material);

        image.lock()->SetSize(glm::vec2{64.f});

        container.lock()->AddChild(image);
    }
}
