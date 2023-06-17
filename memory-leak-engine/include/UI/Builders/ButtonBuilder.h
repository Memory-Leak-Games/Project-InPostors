#pragma once

#include <memory>
namespace mlg {
    class ButtonBuilder {
        std::string name = "Button";

        glm::vec2 size {200.f, 50.f};
        float padding = 10.f;
        std::shared_ptr<class MaterialAsset> defaultMaterial;
        std::shared_ptr<MaterialAsset> focusMaterial;

        std::shared_ptr<class FontAsset> font;
        std::string text = "Button";

        glm::vec3 textColor = glm::vec3{0.f};

        glm::vec2 anchor = glm::vec2{0.5f};

    public:
        ButtonBuilder();

        ButtonBuilder& SetName(const std::string& name);

        ButtonBuilder& SetSize(const glm::vec2& size);
        ButtonBuilder& SetPadding(float padding);
        ButtonBuilder& SetDefaultMaterial(const std::string& path);
        ButtonBuilder& SetFocusMaterial(const std::string& path);

        ButtonBuilder& SetFont(const std::string& path);
        ButtonBuilder& SetText(const std::string& text);
        ButtonBuilder& SetTextColor(const glm::vec3& textColor);

        ButtonBuilder& SetAnchor(const glm::vec2& anchor);

        std::weak_ptr<class Button> Build(class Entity* owner);
    };

}