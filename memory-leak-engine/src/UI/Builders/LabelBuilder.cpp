#include "UI/Builders/LabelBuilder.h"

#include "Gameplay/Entity.h"
#include "UI/Components/Label.h"
#include "UI/Assets/FontAsset.h"

mlg::LabelBuilder::LabelBuilder() {
    font = mlg::AssetManager::GetAsset<mlg::FontAsset>("res/fonts/terminus-bold.ttf");
}

mlg::LabelBuilder& mlg::LabelBuilder::SetName(const std::string& name) {
    this->name = name;
    return *this;
}

mlg::LabelBuilder& mlg::LabelBuilder::SetSize(float size) {
    this->size = size;
    return *this;
}

mlg::LabelBuilder& mlg::LabelBuilder::SetFont(const std::string& path) {
    font = mlg::AssetManager::GetAsset<mlg::FontAsset>(path);
    return *this;
}

mlg::LabelBuilder& mlg::LabelBuilder::SetText(const std::string& text) {
    this->text = text;
    return *this;
}

mlg::LabelBuilder& mlg::LabelBuilder::SetTextColor(const glm::vec3& textColor) {
    this->textColor = textColor;
    return *this;
}

mlg::LabelBuilder& mlg::LabelBuilder::SetAnchor(const glm::vec2& anchor) {
    this->anchor = anchor;
    return *this;
}

mlg::LabelBuilder& mlg::LabelBuilder::SetPadding(float padding) {
    this->padding = padding;
    return *this;
}

mlg::LabelBuilder& mlg::LabelBuilder::SetHorizontalAlignment(mlg::Label::HorizontalAlignment alignment) {
    this->horizontalAlignment = alignment;
    return *this;
}

mlg::LabelBuilder& mlg::LabelBuilder::SetVerticalAlignment(mlg::Label::VerticalAlignment alignment) {
    this->verticalAlignment = alignment;
    return *this;
}

std::weak_ptr<mlg::Label> mlg::LabelBuilder::BuildLabel(Entity* owner) {
    auto label = owner->AddComponent<mlg::Label>(name, font);
    label.lock()->SetSize(size);
    label.lock()->SetText(text);
    label.lock()->SetTextColor(textColor);
    label.lock()->SetAnchor(anchor);
    label.lock()->SetPadding(padding);

    label.lock()->SetHorizontalAlignment(horizontalAlignment);
    label.lock()->SetVerticalAlignment(verticalAlignment);
    
    return label;
}
