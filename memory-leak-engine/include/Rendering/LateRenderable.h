#pragma once

namespace mlg {

    class LateRenderable {
    public:
        virtual void LateDraw(class Renderer* renderer) = 0;
    };

} // mlg
