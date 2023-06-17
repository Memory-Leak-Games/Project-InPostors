#include "include/UI/StorageUI.h"

#include "include/Gameplay/Entity.h"
#include "include/UI/Components/Image.h"
#include "include/UI/Assets/FontAsset.h"
#include "include/Rendering/Assets/MaterialAsset.h"

StorageUI::StorageUI(const std::weak_ptr<mlg::Entity>& owner, const std::string& name) : Component(owner, name) {
    auto font =
            mlg::AssetManager::GetAsset<mlg::FontAsset>(
                    "res/fonts/terminus-bold.ttf");

    auto material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/storage/pin_material.json");
    auto temp = owner.lock()->AddComponent<mlg::Image>("StoragePin", material).lock();
    temp->SetBillboardTarget(owner);
    temp->SetSize({82.f, 82.f});
    temp->SetPosition({0.f, 50.f});

    temp = owner.lock()->AddComponent<mlg::Image>("StoragePin2", material).lock();
    temp->SetBillboardTarget(owner);
    temp->SetSize({68.f, 68.f});
    temp->SetPosition({0.f, 50.f});
    temp->tint = {0.886f, 0.584f, 0.023f, 1.0f};

    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>("res/materials/ui/icon/storage_material.json");
    temp = owner.lock()->AddComponent<mlg::Image>("StorageIcon", material).lock();
    temp->SetSize({20.f, 20.f});
    temp->SetBillboardTarget(owner);
    temp->SetPosition({0.f, 50.f});
}
