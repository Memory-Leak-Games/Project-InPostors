#pragma once

#include "Renderable2D.h"

namespace mlg {
    class Label : Renderable2D {
        uint32_t VAO, VBO;
        std::shared_ptr<class ShaderProgram> shader;
    public:
        glm::vec2 position = {0, 0};
        std::string text;
        std::shared_ptr<class FontAsset> font;
        glm::vec3 textColor = {1, 1, 1};

        Label();
        void Draw(class Renderer2D *renderer) override;
    };
}
