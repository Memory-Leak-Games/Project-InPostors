#pragma once

#include "Renderable2D.h"

namespace mlg {
    class Label : public Renderable2D {
        uint32_t vao, vbo;
        std::shared_ptr<class ShaderProgram> shader;
    public:
        glm::vec2 position = {0, 0};
        std::string text;
        std::shared_ptr<class FontAsset> font;
        glm::vec3 textColor = {1, 1, 1};

        Label();
        void Draw() override;
    };
}
