#pragma once

namespace mlg {

    class Renderable2D {
    public:
        virtual void Draw(const class Renderer2D* renderer) = 0;
    };

}