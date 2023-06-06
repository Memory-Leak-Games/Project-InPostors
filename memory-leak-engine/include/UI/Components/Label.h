#pragma once

#include "UIComponent.h"
#include <glm/fwd.hpp>

namespace mlg {
    class Label : public UIComponent {
    public:
        enum class HorizontalAlignment {
            Left,
            Center,
            Right
        };
        enum class VerticalAlignment {
            Up,
            Center,
            Down
        };
    
    private:
        std::shared_ptr<class FontAsset> font;
        std::shared_ptr<class ShaderProgram> shader;

        std::string text;
        glm::vec3 textColor = {1, 1, 1};

        float scale = 1;

        HorizontalAlignment horizontalAlignment = HorizontalAlignment::Left;
        VerticalAlignment verticalAlignment = VerticalAlignment::Down;

    protected:

        void CalculateActualPosition(const UIRenderer* renderer,const glm::vec2& position) override;
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
        void SetHorizontalAlignment(HorizontalAlignment alignment);
        void SetVerticalAlignment(VerticalAlignment alignment);

        glm::vec2 GetLabelSize() const;
    };
}// namespace mlg
