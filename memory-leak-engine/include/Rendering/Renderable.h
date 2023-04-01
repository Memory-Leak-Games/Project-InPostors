#pragma once

namespace mlg {

    class Renderable {
    public:
        virtual void Draw(class Renderer* renderer) = 0;
        virtual void DrawShadowMap(class Renderer* renderer, class ShaderProgram* shaderProgram) = 0;
    };

} // mlg
