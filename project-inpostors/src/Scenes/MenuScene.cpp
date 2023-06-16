#include "Scenes/MenuScene.h"

#include "Core/AssetManager/AssetManager.h"
#include "Gameplay/Entity.h"
#include "Gameplay/EntityManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "SceneGraph/SceneGraph.h"

#include "UI/Components/Image.h"
#include "UI/Components/UIComponent.h"

MenuScene::~MenuScene() = default;

void MenuScene::Load() {
    auto entity = mlg::EntityManager::SpawnEntity<mlg::Entity>(
            "Image", false, mlg::SceneGraph::GetRoot());

    auto material =
            mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
                    "res/materials/ui/cat_ui_material.json");

    auto image = entity.lock()->AddComponent<mlg::Image>(
            "Image",
            material);

    image.lock()->SetSize({512.f, 512.f});
    image.lock()->SetAnchor(MLG_ANCHOR_CENTER);
    image.lock()->SetPosition(MLG_POS_CENTER);
}
