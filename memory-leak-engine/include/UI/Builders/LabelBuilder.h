#pragma once

#include "UI/Components/Label.h"

namespace mlg {
    class LabelBuilder {
        std::string name = "Label";

        float size = 24.f;
        float padding = 10.f;

        std::shared_ptr<class FontAsset> font;
        std::string text = "Label";

        glm::vec3 textColor = glm::vec3{1.f};
        glm::vec2 anchor = glm::vec2{0.5f};

        mlg::Label::HorizontalAlignment horizontalAlignment =
                mlg::Label::HorizontalAlignment::Center;

        mlg::Label::VerticalAlignment verticalAlignment =
                mlg::Label::VerticalAlignment::Up;

    public:
        LabelBuilder();

        LabelBuilder& SetName(const std::string& name);
        LabelBuilder& SetSize(float size);
        LabelBuilder& SetFont(const std::string& path);
        LabelBuilder& SetText(const std::string& text);
        LabelBuilder& SetTextColor(const glm::vec3& textColor);
        LabelBuilder& SetAnchor(const glm::vec2& anchor);
        LabelBuilder& SetPadding(float padding);
        LabelBuilder& SetHorizontalAlignment(mlg::Label::HorizontalAlignment alignment);
        LabelBuilder& SetVerticalAlignment(mlg::Label::VerticalAlignment alignment);

        std::weak_ptr<class Label> BuildLabel(class Entity* owner);
    };

}// namespace mlg