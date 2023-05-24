#pragma once

#include "UIComponent.h"

namespace mlg {
    class Label : public UIComponent {
        std::shared_ptr<class FontAsset> font;
        std::shared_ptr<class ShaderProgram> shader;

        std::string text;
        glm::vec3 textColor = {1, 1, 1};

        float scale = 1;
    public:

        Label(std::weak_ptr<Entity> owner, std::string name, const std::shared_ptr<class FontAsset>& font);
        void Draw(const UIRenderer* renderer) override;

        [[nodiscard]] const std::string& GetText() const;
        [[nodiscard]] const glm::vec3& GetTextColor() const;
        [[nodiscard]] std::shared_ptr<class FontAsset> GetFont() const;

        void SetText(const std::string& text);
        void SetTextColor(const glm::vec3& textColor);
        void SetFont(const std::shared_ptr<class FontAsset>& font);
        void SetSize(float size);
    };
}
