#include "UI/Builders/ButtonBuilder.h"

#include "Core/AssetManager/AssetManager.h"
#include "Rendering/Assets/MaterialAsset.h"
#include "UI/Assets/FontAsset.h"
#include "UI/Components/Button.h"

#include "Gameplay/Entity.h"
#include "UI/Components/Label.h"
#include <memory>

mlg::ButtonBuilder::ButtonBuilder() {
    defaultMaterial = AssetManager::GetAsset<MaterialAsset>(
            "res/materials/ui/buttons/button_material.json");
    focusMaterial = AssetManager::GetAsset<MaterialAsset>(
            "res/materials/ui/buttons/button_focused_material.json");
    font = AssetManager::GetAsset<FontAsset>(
            "res/fonts/terminus-bold.ttf");
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetName(const std::string& name) {
    this->name = name;
    return *this;
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetSize(const glm::vec2& size) {
    this->size = size;
    return *this;
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetPadding(float padding) {
    this->padding = padding;
    return *this;
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetDefaultMaterial(const std::string& path) {
    defaultMaterial = AssetManager::GetAsset<MaterialAsset>(path);
    return *this;
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetFocusMaterial(const std::string& path) {
    focusMaterial = AssetManager::GetAsset<MaterialAsset>(path);
    return *this;
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetFont(const std::string& path) {
    font = AssetManager::GetAsset<FontAsset>(path);
    return *this;
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetText(const std::string& text) {
    this->text = text;
    return *this;
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetTextColor(const glm::vec3& textColor) {
    this->textColor = textColor;
    return *this;
}

mlg::ButtonBuilder& mlg::ButtonBuilder::SetAnchor(const glm::vec2& anchor) {
    this->anchor = anchor;
    return *this;
}

std::weak_ptr<mlg::Button> mlg::ButtonBuilder::Build(mlg::Entity* owner) {
    auto button = owner->AddComponent<mlg::Button>(
            name, defaultMaterial, focusMaterial, font);
    auto sharedButton = button.lock();
    sharedButton->SetSize(size);
    sharedButton->SetAnchor(anchor);
    sharedButton->SetPadding(padding);

    sharedButton->GetLabel().lock()->SetText(text);
    sharedButton->GetLabel().lock()->SetTextColor(textColor);

    return button;
}


