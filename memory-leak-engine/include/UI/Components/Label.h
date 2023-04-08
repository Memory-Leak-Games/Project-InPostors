#pragma once

#include "UIComponent.h"

namespace mlg {
    class Label : public UIComponent {
        uint32_t vao{}, vbo{};
        std::shared_ptr<class ShaderProgram> shader;
    public:
        std::string text;
        std::shared_ptr<class FontAsset> font;
        glm::vec3 textColor = {1, 1, 1};

        Label(std::weak_ptr<Entity> owner, std::string name);
        void Draw(const Renderer2D* renderer) override;
    };
}
