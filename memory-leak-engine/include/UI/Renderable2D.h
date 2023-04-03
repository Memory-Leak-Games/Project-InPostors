#pragma once

namespace mlg {

    class Renderable2D {
        virtual void Draw(class Renderer2D* renderer) = 0;
    };

}