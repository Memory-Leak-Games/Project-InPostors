#include "UI/Components/DynamicImage.h"
#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "Rendering/Assets/TextureAsset.h"

mlg::DynamicImage::DynamicImage(std::weak_ptr<Entity> owner, std::string name)
    : Image(owner, std::move(name), nullptr) {
    material = mlg::AssetManager::GetAsset<mlg::MaterialAsset>(
            "res/materials/ui/image_material.json");
    material = material->CreateDynamicInstance();

    std::shared_ptr<TextureAsset> texture =
            mlg::AssetManager::GetAsset<mlg::TextureAsset>(
                    "res/textures/error_texture.png");
    material->SetTexture("image", texture);
}

void mlg::DynamicImage::SetTexture(const std::shared_ptr<TextureAsset>& texture) {
    material->SetTexture("image", texture);
}

void mlg::DynamicImage::SetTexture(const std::string& texturePath) {
    auto texture =
            mlg::AssetManager::GetAsset<mlg::TextureAsset>(texturePath);
    SetTexture(texture);
}